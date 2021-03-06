/*
    Copyright (C) 2019 Nicolas Fella <nicolas.fella@gmx.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "callhistorymodel.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <KPeople/PersonData>

CallHistoryModel::CallHistoryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_database(this)
    , m_mapper(new ContactMapper(this))
{
    m_mapper.performInitialScan();

    beginResetModel();
    m_calls = m_database.fetchCalls();
    endResetModel();
}

void CallHistoryModel::addCall(const QString &number, int duration, DialerUtils::CallType type)
{
    beginInsertRows(QModelIndex(), m_calls.size(), m_calls.size());
    m_database.addCall(number, duration, type);

    CallData data;
    data.id = m_database.lastId();
    data.number = number;
    data.duration = duration;
    data.time = QDateTime::currentDateTime();
    data.callType = type;
    m_calls.push_front(data); // insert latest calls at the top of the list

    endInsertRows();
}

void CallHistoryModel::clear()
{
    beginResetModel();
    m_database.clear();
    m_calls.clear();
    endResetModel();
}

QVariant CallHistoryModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    switch (role) {
        case Roles::PhoneNumberRole:
            return m_calls[row].number;
        case DisplayNameRole:
            return KPeople::PersonData{m_mapper.uriForNumber(m_calls.at(index.row()).number)}.name();
        case PhotoRole:
            return KPeople::PersonData{m_mapper.uriForNumber(m_calls.at(index.row()).number)}.photo();
        case Roles::CallTypeRole:
            return m_calls[row].callType;
        case Roles::DurationRole:
            return m_calls[row].duration;
        case Roles::TimeRole:
            return m_calls[row].time;
        case Roles::IdRole:
            return m_calls[row].id;
    }
    return {};
}

int CallHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_calls.size();
}

QHash<int, QByteArray> CallHistoryModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[PhoneNumberRole] = "number";
    roleNames[DisplayNameRole] = "displayName";
    roleNames[PhotoRole] = "photo";
    roleNames[TimeRole] = "time";
    roleNames[DurationRole] = "duration";
    roleNames[CallTypeRole] = "callType";
    roleNames[IdRole] = "dbid";

    return roleNames;
}

bool CallHistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count)

    beginRemoveRows(parent, row, row);
    m_database.remove(m_calls[row].id);
    m_calls = m_database.fetchCalls();
    endRemoveRows();

    return true;
}

void CallHistoryModel::remove(int index)
{
    removeRow(index);
}
