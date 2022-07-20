// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QTimer>
#include <kTelephonyMetaTypes/dialer-types.h>

class ModemController;
class CallUtils;

class CallManager : public QObject
{
    Q_OBJECT
public:
    explicit CallManager(ModemController *modemController, CallUtils *callUtils, QObject *parent = nullptr);

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
    void onCreatedCall(const QString &deviceUni, const QString &callUni);
    void onAccepted(const QString &deviceUni, const QString &callUni);
    void onHungUp(const QString &deviceUni, const QString &callUni);
    void onSendDtmfRequested(const QString &deviceUni, const QString &callUni, const QString &tones);
    void onFetchedCalls();

private:
    ModemController *_modemController;
    CallUtils *_callUtils;
    QTimer _callTimer;
};
