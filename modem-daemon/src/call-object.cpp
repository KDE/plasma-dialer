// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "call-object.h"
#include <QDebug>

constexpr int CALL_DURATION_UPDATE_DELAY = 1000;

CallObject::CallObject(QObject *parent)
    : QObject(parent)
{
    m_callAttemptTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    m_callTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    connect(&m_callAttemptTimer, &QTimer::timeout, this, [this]() {
        m_callData.callAttemptDuration++;
    });
    connect(&m_callTimer, &QTimer::timeout, this, [this]() {
        m_callData.duration++;
    });
}

QString CallObject::id()
{
    return m_callData.id;
}

QString CallObject::protocol()
{
    return m_callData.protocol;
}

QString CallObject::provider()
{
    return m_callData.provider;
}

QString CallObject::account()
{
    return m_callData.account;
}

QString CallObject::communicationWith()
{
    return m_callData.communicationWith;
}

DialerTypes::CallDirection CallObject::direction()
{
    return m_callData.direction;
}

DialerTypes::CallState CallObject::state()
{
    return m_callData.state;
}

DialerTypes::CallStateReason CallObject::stateReason()
{
    return m_callData.stateReason;
}

int CallObject::callAttemptDuration()
{
    return m_callData.callAttemptDuration;
}

QDateTime CallObject::startedAt()
{
    return m_callData.startedAt;
}

int CallObject::duration()
{
    return m_callData.duration;
}

DialerTypes::CallData CallObject::callData()
{
    return m_callData;
}

void CallObject::setId(const QString &id)
{
    m_callData.id = id;
}

void CallObject::setProtocol(const QString &protocol)
{
    m_callData.protocol = protocol;
}

void CallObject::setProvider(const QString &provider)
{
    m_callData.provider = provider;
}

void CallObject::setAccount(const QString &account)
{
    m_callData.account = account;
}

void CallObject::setCommunicationWith(const QString &communicationWith)
{
    m_callData.communicationWith = communicationWith;
}

void CallObject::setDirection(const DialerTypes::CallDirection &direction)
{
    m_callData.direction = direction;
}

void CallObject::setState(const DialerTypes::CallState &state)
{
    m_callData.state = state;
}

void CallObject::setStateReason(const DialerTypes::CallStateReason &stateReason)
{
    m_callData.stateReason = stateReason;
}

void CallObject::setCallAttemptDuration(const int &callAttemptDuration)
{
    m_callData.callAttemptDuration = callAttemptDuration;
}

void CallObject::setStartedAt(const QDateTime &startedAt)
{
    m_callData.startedAt = startedAt;
}

void CallObject::setDuration(const int &duration)
{
    m_callData.duration = duration;
}

void CallObject::onCallStateChanged(const QString &deviceUni,
                                    const QString &callUni,
                                    const DialerTypes::CallDirection &callDirection,
                                    const DialerTypes::CallState &callState,
                                    const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << "new call state:" << deviceUni << callUni << callDirection << callState << callStateReason;
    m_callData.state = callState;
    m_callData.stateReason = callStateReason;
    if (callState == DialerTypes::CallState::RingingIn) {
        qDebug() << "incoming call";
        m_callAttemptTimer.start();
    }
    if (callState == DialerTypes::CallState::Active) {
        qDebug() << "call started";
        m_callTimer.start();
    }
    if (callState == DialerTypes::CallState::Terminated) {
        qDebug() << "call terminated";
        if ((callDirection == DialerTypes::CallDirection::Incoming) && !m_callTimer.isActive()) {
            qDebug() << "missed a call";
        }
        m_callAttemptTimer.stop();
        m_callTimer.stop();
    }
}
