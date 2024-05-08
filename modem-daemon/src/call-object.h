// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QTimer>
#include <kTelephonyMetaTypes/dialer-types.h>

class CallObject : public QObject
{
    Q_OBJECT
public:
    explicit CallObject(QObject *parent = nullptr);

    QString id();

    QString protocol();
    QString provider();
    QString account();

    QString communicationWith();
    DialerTypes::CallDirection direction();
    DialerTypes::CallState state();
    DialerTypes::CallStateReason stateReason();

    int callAttemptDuration();
    QDateTime startedAt();
    int duration();

    DialerTypes::CallData callData();

public Q_SLOTS:
    void onCallStateChanged(const QString &deviceUni,
                            const QString &callUni,
                            const DialerTypes::CallDirection &callDirection,
                            const DialerTypes::CallState &callState,
                            const DialerTypes::CallStateReason &callStateReason);
    void setId(const QString &id);
    void setProtocol(const QString &protocol);
    void setProvider(const QString &provider);
    void setAccount(const QString &account);

    void setCommunicationWith(const QString &communicationWith);
    void setDirection(const DialerTypes::CallDirection &direction);
    void setState(const DialerTypes::CallState &state);
    void setStateReason(const DialerTypes::CallStateReason &stateReason);

    void setCallAttemptDuration(const int &callAttemptDuration);
    void setStartedAt(const QDateTime &startedAt);
    void setDuration(const int &duration);

private:
    DialerTypes::CallData m_callData;
    QTimer m_callAttemptTimer;
    QTimer m_callTimer;
};
