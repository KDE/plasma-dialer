// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "call-object.h"
#include <QDebug>

constexpr int CALL_DURATION_UPDATE_DELAY = 1000;

CallObject::CallObject(QObject *parent)
    : QObject(parent)
{
    _callAttemptTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    _callTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    connect(&_callAttemptTimer, &QTimer::timeout, this, [this]() {
        _callData.callAttemptDuration++;
    });
    connect(&_callTimer, &QTimer::timeout, this, [this]() {
        _callData.duration++;
    });
}

QString CallObject::id()
{
    return _callData.id;
}

QString CallObject::protocol()
{
    return _callData.protocol;
}

QString CallObject::provider()
{
    return _callData.provider;
}

QString CallObject::account()
{
    return _callData.account;
}

QString CallObject::communicationWith()
{
    return _callData.communicationWith;
}

DialerTypes::CallDirection CallObject::direction()
{
    return _callData.direction;
}

DialerTypes::CallState CallObject::state()
{
    return _callData.state;
}

DialerTypes::CallStateReason CallObject::stateReason()
{
    return _callData.stateReason;
}

int CallObject::callAttemptDuration()
{
    return _callData.callAttemptDuration;
}

QDateTime CallObject::startedAt()
{
    return _callData.startedAt;
}

int CallObject::duration()
{
    return _callData.duration;
}

DialerTypes::CallData CallObject::callData()
{
    return _callData;
}

void CallObject::setId(const QString &id)
{
    _callData.id = id;
}

void CallObject::setProtocol(const QString &protocol)
{
    _callData.protocol = protocol;
}

void CallObject::setProvider(const QString &provider)
{
    _callData.provider = provider;
}

void CallObject::setAccount(const QString &account)
{
    _callData.account = account;
}

void CallObject::setCommunicationWith(const QString &communicationWith)
{
    _callData.communicationWith = communicationWith;
}

void CallObject::setDirection(const DialerTypes::CallDirection &direction)
{
    _callData.direction = direction;
}

void CallObject::setState(const DialerTypes::CallState &state)
{
    _callData.state = state;
}

void CallObject::setStateReason(const DialerTypes::CallStateReason &stateReason)
{
    _callData.stateReason = stateReason;
}

void CallObject::setCallAttemptDuration(const int &callAttemptDuration)
{
    _callData.callAttemptDuration = callAttemptDuration;
}

void CallObject::setStartedAt(const QDateTime &startedAt)
{
    _callData.startedAt = startedAt;
}

void CallObject::setDuration(const int &duration)
{
    _callData.duration = duration;
}

void CallObject::onCallStateChanged(const QString &deviceUni,
                                    const QString &callUni,
                                    const DialerTypes::CallDirection &callDirection,
                                    const DialerTypes::CallState &callState,
                                    const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << "new call state:" << deviceUni << callUni << callDirection << callState << callStateReason;
    _callData.state = callState;
    _callData.stateReason = callStateReason;
    if (callState == DialerTypes::CallState::RingingIn) {
        qDebug() << "incoming call";
        _callAttemptTimer.start();
    }
    if (callState == DialerTypes::CallState::Active) {
        qDebug() << "call started";
        _callTimer.start();
    }
    if (callState == DialerTypes::CallState::Terminated) {
        qDebug() << "call terminated";
        if ((callDirection == DialerTypes::CallDirection::Incoming) && !_callTimer.isActive()) {
            qDebug() << "missed a call";
        }
        _callAttemptTimer.stop();
        _callTimer.stop();
    }
}
