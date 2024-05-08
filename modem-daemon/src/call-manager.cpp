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
    m_modemController = modemController;
    m_callUtils = callUtils;

    connect(m_modemController, &ModemController::callStateChanged, this, &CallManager::onCallStateChanged);
    connect(m_modemController, &ModemController::callAdded, this, &CallManager::onCallAdded);
    connect(m_modemController, &ModemController::callDeleted, this, &CallManager::onCallDeleted);

    connect(m_callUtils, &CallUtils::dialed, this, &CallManager::onUtilsCreatedCall);
    connect(m_callUtils, &CallUtils::accepted, this, &CallManager::onUtilsAccepted);
    connect(m_callUtils, &CallUtils::hungUp, this, &CallManager::onUtilsHungUp);
    connect(m_callUtils, &CallUtils::sentDtmf, this, &CallManager::onUtilsSentDtmf);

    connect(m_callUtils, &CallUtils::callsRequested, this, &CallManager::onUtilsCallsRequested);

    callUtils->fetchCalls();
}

void CallManager::onCallAdded(const QString &deviceUni,
                              const QString &callUni,
                              const DialerTypes::CallDirection &callDirection,
                              const DialerTypes::CallState &callState,
                              const DialerTypes::CallStateReason &callStateReason,
                              const QString communicationWith)
{
    qDebug() << "call added:" << deviceUni << callUni;
    m_callUtils->addCall(deviceUni, callUni, callDirection, callState, callStateReason, communicationWith);
}

void CallManager::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    qDebug() << "call deleted:" << deviceUni << callUni;
    m_callUtils->deleteCall(deviceUni, callUni);
}

void CallManager::onCallStateChanged(const DialerTypes::CallData &callData)
{
    qDebug() << "new call state:" << callData.state << callData.stateReason;
    m_callUtils->setCallState(callData);

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

        if (m_inhibitSleepFd) {
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
        m_inhibitSleepFd = fd;
        qDebug() << "logind sleep inhibitor: success";
        break;
    }
    case DialerTypes::CallState::Terminated: {
        QString deviceUni; // TODO: improve deviceUni getter
        m_modemController->deleteCall(deviceUni, callData.id);
        m_inhibitSleepFd.reset();
        qDebug() << "logind sleep inhibitor: turned off";
        break;
    }
    default:
        break;
    }
}

void CallManager::onUtilsCreatedCall(const QString &deviceUni, const QString &callUni)
{
    m_modemController->createCall(deviceUni, callUni);
}

void CallManager::onUtilsAccepted(const QString &deviceUni, const QString &callUni)
{
    m_modemController->acceptCall(deviceUni, callUni);
}

void CallManager::onUtilsHungUp(const QString &deviceUni, const QString &callUni)
{
    m_modemController->hangUp(deviceUni, callUni);
}

void CallManager::onUtilsSentDtmf(const QString &deviceUni, const QString &callUni, const QString &tones)
{
    m_modemController->sendDtmf(deviceUni, callUni, tones);
}

void CallManager::onUtilsCallsRequested()
{
    m_callUtils->setCalls(m_modemController->fetchCalls());
}
