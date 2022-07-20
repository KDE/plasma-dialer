// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include "call-model.h"
#include "declarative-call-utils.h"
#include <QTimer>

class ActiveCallModel : public CallModel
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(DialerTypes::CallState callState READ callState NOTIFY callStateChanged)
    Q_PROPERTY(uint callDuration READ callDuration NOTIFY callDurationChanged)

public:
    ActiveCallModel(QObject *parent = nullptr);

    bool active() const;
    void setActive(bool newActive);
    DialerTypes::CallState callState() const;
    void setCallState(const DialerTypes::CallState state);
    uint callDuration() const;
    void setCallDuration(const uint duration);

public Q_SLOTS:
    Q_INVOKABLE void sendDtmf(const QString &tones);
    Q_INVOKABLE void dial(const QString &deviceUni, const QString &number);
    Q_INVOKABLE QString activeCallUni();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    void activeChanged();
    void callStateChanged();
    void callDurationChanged();

private Q_SLOTS:
    void onCallAdded(const QString &deviceUni,
                     const QString &callUni,
                     const DialerTypes::CallDirection &callDirection,
                     const DialerTypes::CallState &callState,
                     const DialerTypes::CallStateReason &callStateReason,
                     const QString communicationWith);
    void onCallDeleted(const QString &deviceUni, const QString &callUni);
    void onCallStateChanged(const QString &deviceUni,
                            const QString &callUni,
                            const DialerTypes::CallDirection &callDirection,
                            const DialerTypes::CallState &callState,
                            const DialerTypes::CallStateReason &callStateReason);
    void onFetchedCallsChanged(const DialerTypes::CallDataVector &fetchedCalls);
    void onCallDurationChanged(const int duration);

private:
    DeclarativeCallUtils *_callUtils;
    DialerTypes::CallDataVector _calls;
    QTimer _callsTimer;
    bool _active = false;
    DialerTypes::CallState _callState = DialerTypes::CallState::Unknown;
    uint _callDuration = 0;
};
