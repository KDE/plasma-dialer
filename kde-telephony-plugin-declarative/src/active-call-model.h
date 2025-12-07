// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include "call-model.h"
#include "callutilsinterface.h"
#include <QTimer>

class ActiveCallModel : public CallModel
{
    Q_OBJECT
    Q_PROPERTY(bool incoming READ incoming NOTIFY incomingChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool inCall READ inCall NOTIFY inCallChanged)
    Q_PROPERTY(QString communicationWith READ communicationWith NOTIFY communicationWithChanged)
    Q_PROPERTY(qulonglong duration READ duration NOTIFY durationChanged)

public:
    ActiveCallModel(QObject *parent = nullptr);

    // Whether any sort of call is ongoing (includes waiting incoming or outgoing calls)
    bool active() const;
    void setActive(bool newActive);

    // Whether a call is ongoing, where both ends have picked up
    bool inCall() const;
    void setInCall(bool inCall);

    bool incoming() const;
    void setIncoming(bool newIncoming);

    QString communicationWith() const;
    void setCommunicationWith(const QString communicationWith);

    qulonglong duration() const;
    void setDuration(qulonglong duration);

    void setCallUtils(org::kde::telephony::CallUtils *callUtils);

public Q_SLOTS:
    Q_INVOKABLE void sendDtmf(const QString &tones);
    Q_INVOKABLE void dial(const QString &deviceUni, const QString &number);
    Q_INVOKABLE QString activeCallUni();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    void activeChanged();
    void inCallChanged();
    void incomingChanged();
    void communicationWithChanged();
    void durationChanged();

private Q_SLOTS:
    void onUtilsCallAdded(const QString &deviceUni,
                          const QString &callUni,
                          const DialerTypes::CallDirection &callDirection,
                          const DialerTypes::CallState &callState,
                          const DialerTypes::CallStateReason &callStateReason,
                          const QString communicationWith);
    void onUtilsCallDeleted(const QString &deviceUni, const QString &callUni);
    void onUtilsCallStateChanged(const DialerTypes::CallData &callData);
    void onUtilsCallsChanged(const DialerTypes::CallDataVector &fetchedCalls);

private:
    DialerTypes::CallData findActiveCall();
    void updateActiveCallProps();

    org::kde::telephony::CallUtils *m_callUtils;
    DialerTypes::CallDataVector m_calls;
    QTimer m_callsTimer;
    bool m_active = false;
    bool m_inCall = false;
    bool m_incoming = false;
    QString m_communicationWith;
    qulonglong m_duration;

    void updateTimers();
};
