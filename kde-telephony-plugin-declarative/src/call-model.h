// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <kTelephonyMetaTypes/dialer-types.h>

class CallModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CallModel(QObject *parent = nullptr);

    enum Roles {
        EventRole = Qt::UserRole + 1,
        ProtocolRole,
        AccountRole,
        ProviderRole,
        CommunicationWithRole,
        DirectionRole,
        StateRole,
        StateReasonRole,
        CallAttemptDurationRole,
        StartedAtRole,
        DurationRole
    };
    Q_ENUM(Roles)

    QHash<int, QByteArray> roleNames() const override;
};
