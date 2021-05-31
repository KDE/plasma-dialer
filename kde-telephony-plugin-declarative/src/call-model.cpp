// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "call-model.h"

CallModel::CallModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> CallModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[EventRole] = "event";
    roleNames[ProtocolRole] = "protocol";
    roleNames[AccountRole] = "account";
    roleNames[ProviderRole] = "provider";
    roleNames[CommunicationWithRole] = "communicationWith";
    roleNames[DirectionRole] = "direction";
    roleNames[StateRole] = "state";
    roleNames[StateReasonRole] = "stateReason";
    roleNames[CallAttemptDurationRole] = "callAttemptDuration";
    roleNames[StartedAtRole] = "startedAt";
    roleNames[DurationRole] = "duration";
    return roleNames;
}
