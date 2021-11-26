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

public:
    ActiveCallModel(QObject *parent = nullptr);

    bool active() const;
    void setActive(bool newActive);

public Q_SLOTS:
    Q_INVOKABLE void sendDtmf(const QString &tones);
    Q_INVOKABLE void dial(const QString &deviceUni, const QString &number);
    Q_INVOKABLE QString activeCallUni();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    void activeChanged();

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

private:
    DeclarativeCallUtils *_callUtils;
    DialerTypes::CallDataVector _calls;
    QTimer _callsTimer;
    bool _active = false;
};
