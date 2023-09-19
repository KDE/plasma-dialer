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
    Q_PROPERTY(QString communicationWith READ communicationWith NOTIFY communicationWithChanged)
    Q_PROPERTY(qulonglong duration READ duration NOTIFY durationChanged)

public:
    ActiveCallModel(QObject *parent = nullptr);

    bool active() const;
    void setActive(bool newActive);

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
    org::kde::telephony::CallUtils *_callUtils;
    DialerTypes::CallDataVector _calls;
    QTimer _callsTimer;
    bool _active = false;
    bool _incoming = false;
    QString _communicationWith;
    qulonglong _duration;

    void _updateTimers();
};
