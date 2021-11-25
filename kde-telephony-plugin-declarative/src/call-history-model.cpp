// SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "call-history-model.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

CallHistoryModel::CallHistoryModel(QObject *parent)
    : CallModel(parent)
{
    _databaseInterface = new org::kde::telephony::CallHistoryDatabase(QString::fromLatin1(_databaseInterface->staticInterfaceName()),
                                                                      QStringLiteral("/org/kde/telephony/CallHistoryDatabase/tel/mm"),
                                                                      QDBusConnection::sessionBus(),
                                                                      this);

    if (!_databaseInterface->isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallHistoryDatabase interface";
        return;
    }

    beginResetModel();
    _fetchCalls();
    endResetModel();

    /*
    connect(&ContactPhoneNumberMapper::instance(), &ContactPhoneNumberMapper::contactsChanged, this, [this] {
        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1), {DisplayNameRole, PhotoRole});
    });
    */

    connect(_databaseInterface, &org::kde::telephony::CallHistoryDatabase::callsChanged, this, [this] {
        beginResetModel();
        _fetchCalls();
        endResetModel();
    });
}

void CallHistoryModel::addCall(const DialerTypes::CallData &callData)
{
    QDBusPendingReply<int> reply = _databaseInterface->lastId();
    reply.waitForFinished();
    int databaseLastId;
    if (reply.isValid()) {
        databaseLastId = reply.value();
    } else {
        qDebug() << Q_FUNC_INFO << reply.error();
        return;
    }

    beginInsertRows(QModelIndex(), _calls.size(), _calls.size());
    _databaseInterface->addCall(callData);

    DialerTypes::CallData data;
    data.id = databaseLastId;

    _calls.push_front(callData); // insert latest calls at the top of the list

    endInsertRows();
}

void CallHistoryModel::clear()
{
    auto reply = _databaseInterface->clear();
    reply.waitForFinished();
    if (!reply.isValid()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        return;
    }
    beginResetModel();
    _calls.clear();
    endResetModel();
}

bool CallHistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count)
    auto reply = _databaseInterface->remove(_calls[row].id);
    reply.waitForFinished();
    if (!reply.isValid()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        return false;
    }

    beginRemoveRows(parent, row, row);
    _fetchCalls();
    endRemoveRows();

    return true;
}

void CallHistoryModel::_fetchCalls()
{
    QDBusPendingReply<DialerTypes::CallDataVector> reply = _databaseInterface->fetchCalls();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    _calls = reply;
}

void CallHistoryModel::remove(int index)
{
    removeRow(index);
}

QVariant CallHistoryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    switch (role) {
    case Roles::EventRole:
        return _calls[row].id;
    case Roles::ProtocolRole:
        return _calls[row].protocol;
    case Roles::AccountRole:
        return _calls[row].account;
    case Roles::ProviderRole:
        return _calls[row].provider;
    case Roles::CommunicationWithRole:
        return _calls[row].communicationWith;
    case Roles::DirectionRole:
        return QVariant::fromValue(_calls[row].direction);
    case Roles::StateRole:
        return QVariant::fromValue(_calls[row].state);
    case Roles::StateReasonRole:
        return QVariant::fromValue(_calls[row].stateReason);
    case Roles::CallAttemptDurationRole:
        return _calls[row].callAttemptDuration;
    case Roles::StartedAtRole:
        return _calls[row].startedAt;
    case Roles::DurationRole:
        return _calls[row].duration;
    }
    return {};
}

int CallHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _calls.size();
}
