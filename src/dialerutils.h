/*
 * Copyright 2015 Marco Martin <mart@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.

 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**/

#ifndef DIALERUTILS_H
#define DIALERUTILS_H

#include <QObject>
#include <QPointer>

#include <TelepathyQt/Account>
#include <qofono-qt5/qofonomessagewaiting.h>

class DialerUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CallState callState READ callState NOTIFY callStateChanged);
    Q_PROPERTY(uint callDuration READ callDuration NOTIFY callDurationChanged);
    Q_PROPERTY(QString callContactAlias READ callContactAlias NOTIFY callContactAliasChanged);
    Q_PROPERTY(QString callContactNumber READ callContactNumber NOTIFY callContactNumberChanged);
    Q_PROPERTY(bool isIncomingCall READ isIncomingCall NOTIFY isIncomingCallChanged);
    Q_PROPERTY(QString callContactDisplayString READ callContactDisplayString NOTIFY callContactDisplayStringChanged);

public:
    enum CallState {
        Idle,
        Dialing,
        Incoming,
        Answered,
        Active,
        Ended,
        Failed,
    };
    Q_ENUM(CallState)

    enum CallType {
        IncomingRejected,
        IncomingAccepted,
        Outgoing,
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

    QString callContactDisplayString() const;

    Q_INVOKABLE void resetMissedCalls();
    Q_INVOKABLE void dial(const QString &number);
    Q_INVOKABLE QString formatNumber(const QString &number);
    Q_INVOKABLE void getImeis();
    Q_INVOKABLE QString getVoicemailNumber();

Q_SIGNALS:
    void missedCallsActionTriggered();
    void callStateChanged();
    void callDurationChanged();
    void callContactAliasChanged();
    void callContactNumberChanged();
    void isIncomingCallChanged();
    void callContactDisplayStringChanged();
    void acceptCall();
    void rejectCall();
    void hangUp();
    void callEnded(const QString &callContactNumber, uint callDuration, bool isIncomingCall);
    void sendDtmf(const QString &tones);
    void initiateUssd(const QString &command);
    void respondToUssd(const QString &reply);
    void cancelUssd();
    void notificationReceivedFromUssd(const QString &message);
    void requestReceivedFromUssd(const QString &message);
    void displayImeis(const QStringList &imeis);

private:
    int m_missedCalls = 0;
    CallState m_callState = CallState::Idle;
    Tp::AccountPtr m_simAccount;
    QString m_callContactNumber;
    uint m_callDuration = 0;
    QString m_callContactAlias;
    bool m_isIncomingCall;
    QOfonoMessageWaiting *m_msgWaiting = nullptr;
    QString m_voicemailNumber;
};

Q_DECLARE_METATYPE(DialerUtils::CallType)

#endif
