/*
 * SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
**/

#ifndef DIALERUTILS_H
#define DIALERUTILS_H

#include <QObject>
#include <QPointer>
#include <KNotification>

#include <TelepathyQt/Account>

class DialerUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CallState callState READ callState NOTIFY callStateChanged);
    Q_PROPERTY(uint callDuration READ callDuration NOTIFY callDurationChanged);
    Q_PROPERTY(QString callContactAlias READ callContactAlias NOTIFY callContactAliasChanged);
    Q_PROPERTY(QString callContactNumber READ callContactNumber NOTIFY callContactNumberChanged);
    Q_PROPERTY(bool isIncomingCall READ isIncomingCall NOTIFY isIncomingCallChanged);

public:
    enum CallState {
        Idle,
        Dialing,
        Incoming,
        Answered,
        Active,
        Ended,
        Failed
    };
    Q_ENUM(CallState)

    enum CallType {
        IncomingRejected,
        IncomingAccepted,
        Outgoing
    };
    Q_ENUM(CallType)

    DialerUtils(const Tp::AccountPtr &simAccount, QObject *parent = nullptr);
    ~DialerUtils() override;

    CallState callState() const;
    void setCallState(const DialerUtils::CallState state);

    uint callDuration() const;
    void setCallDuration(uint duration);

    QString callContactAlias() const;
    void setCallContactAlias(const QString &contactAlias);

    QString callContactNumber() const;
    void setCallContactNumber(const QString &contactNumber);

    bool isIncomingCall() const;
    void setIsIncomingCall(bool isIncomingCall);

    void emitCallEnded();

    Q_INVOKABLE void resetMissedCalls();
    Q_INVOKABLE void dial(const QString &number);
    Q_INVOKABLE QString formatNumber(const QString& number);

Q_SIGNALS:
    void missedCallsActionTriggered();
    void callStateChanged();
    void callDurationChanged();
    void callContactAliasChanged();
    void callContactNumberChanged();
    void isIncomingCallChanged();
    void acceptCall();
    void rejectCall();
    void hangUp();
    void callEnded(const QString &callContactNumber, uint callDuration, bool isIncomingCall);

private:
    QPointer <KNotification> m_callsNotification;
    QPointer <KNotification> m_ringingNotification;
    int m_missedCalls;
    CallState m_callState;
    Tp::AccountPtr m_simAccount;
    QString m_callContactNumber;
    uint m_callDuration;
    QString m_callContactAlias;
    bool m_isIncomingCall;
};

Q_DECLARE_METATYPE(DialerUtils::CallType)

#endif
