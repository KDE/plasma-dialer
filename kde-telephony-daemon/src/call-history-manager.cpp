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
    m_callHistoryDatabase = callHistoryDatabase;
}

void CallHistoryManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    m_callUtils = callUtils;

    connect(m_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &CallHistoryManager::onCallStateChanged);
}

void CallHistoryManager::onCallStateChanged(const DialerTypes::CallData &callData)
{
    qDebug() << "new call state:" << callData.state << callData.stateReason;
    if (callData.state == DialerTypes::CallState::Terminated) {
        m_callHistoryDatabase->addCall(callData);
    }
}
