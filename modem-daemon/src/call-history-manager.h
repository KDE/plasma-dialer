// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <kTelephonyMetaTypes/dialer-types.h>

class ModemController;
class CallHistoryDatabase;

class CallHistoryManager : public QObject
{
    Q_OBJECT
public:
    explicit CallHistoryManager(ModemController *modemController, CallHistoryDatabase *callHistoryDatabase, QObject *parent = nullptr);

private Q_SLOTS:
    void onStateChanged(const QString &deviceUni,
                        const QString &callUni,
                        const DialerTypes::CallDirection &callDirection,
                        const DialerTypes::CallState &callState,
                        const DialerTypes::CallStateReason &callStateReason);

private:
    ModemController *_modemController;
    CallHistoryDatabase *_callHistoryDatabase;
};
