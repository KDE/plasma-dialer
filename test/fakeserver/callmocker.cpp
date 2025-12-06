// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "callmocker.h"
#include "calladaptor.h"

#include <QDBusConnection>

using namespace Qt::StringLiterals;

CallMocker::CallMocker(int num, QVariantMap props, QObject *parent)
    : QObject{parent}
    , m_props{props}
{
    setDBusName(QStringLiteral("org.freedesktop.ModemManager1.Call"));
    setDBusPath(QDBusObjectPath(QStringLiteral("/org/freedesktop/ModemManager1/Call/") + QString::number(num)));

    m_number = props[u"number"_s].toString();

    if (props[u"direction"_s] == u"outgoing"_s) {
        m_callState = MM_CALL_STATE_DIALING;
        m_callStateReason = MM_CALL_STATE_REASON_UNKNOWN;
        m_callDirection = MM_CALL_DIRECTION_OUTGOING;

        QTimer::singleShot(1000, this, [this]() {
            setState(MM_CALL_STATE_RINGING_OUT, MM_CALL_STATE_REASON_UNKNOWN);
            qDebug() << "Outgoing call ringing...";

            QTimer::singleShot(3000, this, [this]() {
                qDebug() << "Outgoing call active.";
                setState(MM_CALL_STATE_ACTIVE, MM_CALL_STATE_REASON_OUTGOING_STARTED);
            });
        });
    } else {
        m_callState = MM_CALL_STATE_RINGING_IN;
        m_callStateReason = MM_CALL_STATE_REASON_INCOMING_NEW;
        m_callDirection = MM_CALL_DIRECTION_INCOMING;
    }
}

void CallMocker::registerObject()
{
    if (!m_initialized) {
        qDebug() << "Initializing dbus mock for org.freedesktop.ModemManager1.Call...";

        new CallAdaptor{this};

        auto bus = QDBusConnection::systemBus();
        bool status = bus.registerObject(dbusPath().path(), this, QDBusConnection::ExportAdaptors);

        if (status) {
            m_initialized = true;
        } else {
            qDebug() << "Failed to init!";
        }
    }
}

int CallMocker::state() const
{
    return m_callState; // MMCallState
}

void CallMocker::setState(int state, int reason)
{
    int oldState = m_callState;
    m_callState = state;
    m_callStateReason = reason;
    Q_EMIT StateChanged(oldState, state, reason);
    Q_EMIT stateChanged2();
    Q_EMIT stateReasonChanged();
}

int CallMocker::stateReason() const
{
    return m_callStateReason; // MMCallStateReason
}

int CallMocker::direction() const
{
    return m_callDirection; // MMCallDirection
}

QString CallMocker::number() const
{
    return m_number;
}

void CallMocker::Start()
{
    qDebug() << "Call: start";
}

void CallMocker::Accept()
{
    qDebug() << "Call: accept";

    if (m_props[u"direction"_s] == u"outgoing"_s) {
        qDebug() << "Cannot accept an outgoing call!";
        return;
    }

    setState(MM_CALL_STATE_ACTIVE, MM_CALL_STATE_REASON_ACCEPTED);
}

void CallMocker::Hangup()
{
    qDebug() << "Call: hangup";
    Q_EMIT hangupRequested();
}

void CallMocker::SendDtmf(const QString &dtmf)
{
    qDebug() << "Call: send dtmf";
}
