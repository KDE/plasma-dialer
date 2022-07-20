// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>

#include "call-object.h"
#include <kTelephonyMetaTypes/dialer-types.h>

class CallUtils : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.CallUtils")

public:
    CallUtils(QObject *parent = nullptr);
    ~CallUtils() override;

public Q_SLOTS:
    QString formatNumber(const QString &number);
    void dial(const QString &deviceUni, const QString &number);
    void accept(const QString &deviceUni, const QString &callUni);
    void hangUp(const QString &deviceUni, const QString &callUni);
    void sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones);
    void fetchCalls();
    void setFetchedCalls(const DialerTypes::CallDataVector &fetchedCalls);

Q_SIGNALS:
    void dialed(const QString &deviceUni, const QString &number);
    void accepted(const QString &deviceUni, const QString &callUni);
    void hungUp(const QString &deviceUni, const QString &callUni);
    void sentDtmf(const QString &deviceUni, const QString &callUni, const QString &tones);
    void fetchedCalls();
    void fetchedCallsChanged(const DialerTypes::CallDataVector &fetchedCalls);
    void callAdded(const QString &deviceUni,
                   const QString &callUni,
                   const DialerTypes::CallDirection &callDirection,
                   const DialerTypes::CallState &callState,
                   const DialerTypes::CallStateReason &callStateReason,
                   const QString communicationWith);
    void callDeleted(const QString &deviceUni, const QString &callUni);
    void callStateChanged(const QString &deviceUni,
                          const QString &callUni,
                          const DialerTypes::CallDirection &callDirection,
                          const DialerTypes::CallState &callState,
                          const DialerTypes::CallStateReason &callStateReason);
    void callDurationChanged(const int duration);
};
