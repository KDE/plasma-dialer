// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <optional>

#include <QDBusUnixFileDescriptor>
#include <QObject>
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
    void onCallStateChanged(const DialerTypes::CallData &callData);
    void onUtilsCreatedCall(const QString &deviceUni, const QString &callUni);
    void onUtilsAccepted(const QString &deviceUni, const QString &callUni);
    void onUtilsHungUp(const QString &deviceUni, const QString &callUni);
    void onUtilsSentDtmf(const QString &deviceUni, const QString &callUni, const QString &tones);
    void onUtilsCallsRequested();

private:
    ModemController *m_modemController;
    CallUtils *m_callUtils;

    std::optional<QDBusUnixFileDescriptor> m_inhibitSleepFd;
};
