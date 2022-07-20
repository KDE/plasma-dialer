// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "call-manager.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <optional>

#include "call-utils.h"
#include "modem-controller.h"

constexpr int CALL_DURATION_UPDATE_DELAY = 1000;

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

    // TODO (Alexander Trofimov): Timer should not be created in CallManager
    // Call Manager should receive signal CallTimeChanged and react on it with onCallTimeChanged
    _callTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    connect(&_callTimer, &QTimer::timeout, this, [this]() {
        Q_EMIT _callUtils->callDurationChanged(_modemController->getCallDuration());
    });
}

void CallManager::onCallAdded(const QString &deviceUni,
                              const QString &callUni,
                              const DialerTypes::CallDirection &callDirection,
                              const DialerTypes::CallState &callState,
                              const DialerTypes::CallStateReason &callStateReason,
                              const QString communicationWith)
{
    qDebug() << "call added:" << deviceUni << callUni << communicationWith;
    Q_EMIT _callUtils->callAdded(deviceUni, callUni, callDirection, callState, callStateReason, communicationWith);
}

void CallManager::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    qDebug() << "call deleted:" << deviceUni << callUni;
    Q_EMIT _callUtils->callDeleted(deviceUni, callUni);
}

void CallManager::onCallStateChanged(const QString &deviceUni,
                                     const QString &callUni,
                                     const DialerTypes::CallDirection &callDirection,
                                     const DialerTypes::CallState &callState,
                                     const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << "new call state:" << deviceUni << callUni << callDirection << callState << callStateReason;

    if (callState == DialerTypes::CallState::Active) {
        _callTimer.start();
    } else if (callState == DialerTypes::CallState::Terminated) {
        _callTimer.stop();
    }
    Q_EMIT _callUtils->callStateChanged(deviceUni, callUni, callDirection, callState, callStateReason);
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
