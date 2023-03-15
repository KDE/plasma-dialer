// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <kTelephonyMetaTypes/dialer-types.h>

#include "callutilsinterface.h"

class ModemController;
class CallHistoryDatabase;

class CallHistoryManager : public QObject
{
    Q_OBJECT
public:
    explicit CallHistoryManager(QObject *parent = nullptr);

    void setCallHistoryDatabase(CallHistoryDatabase *callHistoryDatabase);
    void setCallUtils(org::kde::telephony::CallUtils *callUtils);

private Q_SLOTS:
    void onCallStateChanged(const QString &deviceUni,
                            const QString &callUni,
                            const DialerTypes::CallDirection &callDirection,
                            const DialerTypes::CallState &callState,
                            const DialerTypes::CallStateReason &callStateReason);

private:
    org::kde::telephony::CallUtils *_callUtils;

    ModemController *_modemController;
    CallHistoryDatabase *_callHistoryDatabase;
};
