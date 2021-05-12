// SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <QVector>

#include "database.h"
#include "dialerutils.h"

class CallHistoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CallHistoryModel(QObject *parent = nullptr);

    enum Roles {
        PhoneNumberRole = Qt::UserRole + 1,
        DisplayNameRole,
        PhotoRole,
        DurationRole,
        TimeRole,
        CallTypeRole,
        IdRole,
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addCall(const QString &number, int duration, DialerUtils::CallType type);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<CallData> m_calls;
    Database m_database;
};
