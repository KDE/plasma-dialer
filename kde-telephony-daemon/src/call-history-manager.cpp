// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "call-history-manager.h"

#include <QtDebug>

#include "call-history-database.h"

CallHistoryManager::CallHistoryManager(QObject *parent)
    : QObject(parent)
{
}

void CallHistoryManager::setCallHistoryDatabase(CallHistoryDatabase *callHistoryDatabase)
{
    _callHistoryDatabase = callHistoryDatabase;
}

void CallHistoryManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    _callUtils = callUtils;

    connect(_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &CallHistoryManager::onCallStateChanged);
}

void CallHistoryManager::onCallStateChanged(const QString &deviceUni,
                                            const QString &callUni,
                                            const DialerTypes::CallDirection &callDirection,
                                            const DialerTypes::CallState &callState,
                                            const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << "new call state:" << deviceUni << callUni << callDirection << callState << callStateReason;
    if (callState == DialerTypes::CallState::Terminated) {
        // FIXME: callUtils API with DialerTypes::CallData ?
        // _callHistoryDatabase->addCall(_callUtils->getCall(deviceUni, callUni));
        // FIXME: delete call via call-manager?
        // _callUtils->deleteCall(deviceUni, callUni);
    }
}
