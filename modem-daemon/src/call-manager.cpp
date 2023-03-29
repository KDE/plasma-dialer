// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "call-manager.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QStringLiteral>

#include "call-utils.h"
#include "modem-controller.h"

CallManager::CallManager(ModemController *modemController, CallUtils *callUtils, QObject *parent)
    : QObject(parent)
{
    _modemController = modemController;
    _callUtils = callUtils;

    connect(_modemController, &ModemController::callStateChanged, this, &CallManager::onCallStateChanged);
    connect(_modemController, &ModemController::callAdded, this, &CallManager::onCallAdded);
    connect(_modemController, &ModemController::callDeleted, this, &CallManager::onCallDeleted);

    connect(_callUtils, &CallUtils::dialed, this, &CallManager::onCreatedCall);
    connect(_callUtils, &CallUtils::accepted, this, &CallManager::onAccepted);
    connect(_callUtils, &CallUtils::hungUp, this, &CallManager::onHungUp);
    connect(_callUtils, &CallUtils::sentDtmf, this, &CallManager::onSendDtmfRequested);
    connect(_callUtils, &CallUtils::fetchedCalls, this, &CallManager::onFetchedCalls);
}

void CallManager::onCallAdded(const QString &deviceUni,
                              const QString &callUni,
                              const DialerTypes::CallDirection &callDirection,
                              const DialerTypes::CallState &callState,
                              const DialerTypes::CallStateReason &callStateReason,
                              const QString communicationWith)
{
    qDebug() << "call added:" << deviceUni << callUni;
    Q_EMIT _callUtils->callAdded(deviceUni, callUni, callDirection, callState, callStateReason, communicationWith);
}

void CallManager::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    qDebug() << "call deleted:" << deviceUni << callUni;
    Q_EMIT _callUtils->callDeleted(deviceUni, callUni);
}

void CallManager::onCallStateChanged(const DialerTypes::CallData &callData)
{
    qDebug() << "new call state:" << callData.state << callData.stateReason;
    Q_EMIT _callUtils->callStateChanged(callData);

    // Add inhibition in logind when call is active.
    // Otherwise if powerdevil is configured to suspend device afer few minutes,
    // it will happily put it to suspend, freezing calls.
    //
    // For XDG spec, see also:
    // https://gitlab.freedesktop.org/xdg/xdg-specs/-/issues/99
    //
    // For Solid support state, see also:
    // https://invent.kde.org/frameworks/solid/-/blob/79bdd41abcd479f486976596fcca40b388caa9b2/CMakeLists.txt#L97-L104
    switch (callData.state) {
    case DialerTypes::CallState::Active: {
        qDebug() << "logind sleep inhibitor: starting";

        if (_inhibitSleepFd) {
            qDebug() << "logind sleep inhibitor: already inhibited";
            break;
        }

        auto bus = QDBusConnection::systemBus();

        if (!bus.isConnected()) {
            qDebug() << "logind sleep inhibitor: error: D-Bus system bus is not connected";
            break;
        }

        QDBusMessage sleepInhibitCall = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.login1"),
                                                                       QStringLiteral("/org/freedesktop/login1"),
                                                                       QStringLiteral("org.freedesktop.login1.Manager"),
                                                                       QStringLiteral("Inhibit"));
        sleepInhibitCall.setArguments(
            {QStringLiteral("sleep"), QCoreApplication::instance()->applicationName(), tr("Active call inhibits system suspend"), QStringLiteral("block")});
        QDBusReply<QDBusUnixFileDescriptor> reply = bus.call(sleepInhibitCall);

        if (!reply.isValid()) {
            qDebug() << "logind sleep inhibitor: error: call failed";
            break;
        }

        auto const fd = reply.value();
        if (!fd.isValid()) {
            qDebug() << "logind sleep inhibitor: error: call returned an invalid file descriptor";
            break;
        }
        _inhibitSleepFd = fd;
        qDebug() << "logind sleep inhibitor: success";
        break;
    }
    case DialerTypes::CallState::Terminated: {
        // TODO: FIXME: not cool yet
        auto deviceUni = _modemController->deviceUni(callData.provider);
        _modemController->deleteCall(deviceUni, callData.id);
        _inhibitSleepFd.reset();
        qDebug() << "logind sleep inhibitor: turned off";
        break;
    }
    default:
        break;
    }
}

void CallManager::onCreatedCall(const QString &deviceUni, const QString &callUni)
{
    _modemController->createCall(deviceUni, callUni);
}

void CallManager::onAccepted(const QString &deviceUni, const QString &callUni)
{
    _modemController->acceptCall(deviceUni, callUni);
}

void CallManager::onHungUp(const QString &deviceUni, const QString &callUni)
{
    _modemController->hangUp(deviceUni, callUni);
}

void CallManager::onSendDtmfRequested(const QString &deviceUni, const QString &callUni, const QString &tones)
{
    _modemController->sendDtmf(deviceUni, callUni, tones);
}

void CallManager::onFetchedCalls()
{
    _callUtils->setFetchedCalls(_modemController->fetchCalls());
}
