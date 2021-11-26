// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "call-history-manager.h"

#include <QtDebug>

#include "call-history-database.h"
#include "modem-controller.h"

CallHistoryManager::CallHistoryManager(ModemController *modemController, CallHistoryDatabase *callHistoryDatabase, QObject *parent)
    : QObject(parent)
{
    _modemController = modemController;
    _callHistoryDatabase = callHistoryDatabase;

    connect(_modemController, &ModemController::callStateChanged, this, &CallHistoryManager::onStateChanged);
}

void CallHistoryManager::onStateChanged(const QString &deviceUni,
                                        const QString &callUni,
                                        const DialerTypes::CallDirection &callDirection,
                                        const DialerTypes::CallState &callState,
                                        const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << "new call state:" << deviceUni << callUni << callDirection << callState << callStateReason;
    if (callState == DialerTypes::CallState::Terminated) {
        _callHistoryDatabase->addCall(_modemController->getCall(deviceUni, callUni));
        _modemController->deleteCall(deviceUni, callUni);
    }
}
