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
    m_databaseInterface = new org::kde::telephony::CallHistoryDatabase(QString::fromLatin1(m_databaseInterface->staticInterfaceName()),
                                                                       QStringLiteral("/org/kde/telephony/CallHistoryDatabase/tel/mm"),
                                                                       QDBusConnection::sessionBus(),
                                                                       this);

    if (!m_databaseInterface->isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallHistoryDatabase interface";
        return;
    }

    beginResetModel();
    fetchCalls();
    endResetModel();

    /*
    connect(&ContactPhoneNumberMapper::instance(), &ContactPhoneNumberMapper::contactsChanged, this, [this] {
        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1), {DisplayNameRole, PhotoRole});
    });
    */

    connect(m_databaseInterface, &org::kde::telephony::CallHistoryDatabase::callsChanged, this, [this] {
        beginResetModel();
        fetchCalls();
        endResetModel();
    });
}

void CallHistoryModel::addCall(const DialerTypes::CallData &callData)
{
    QDBusPendingReply<int> reply = m_databaseInterface->lastId();
    reply.waitForFinished();
    int databaseLastId;
    if (reply.isValid()) {
        databaseLastId = reply.value();
    } else {
        qDebug() << Q_FUNC_INFO << reply.error();
        return;
    }

    beginInsertRows(QModelIndex(), m_calls.size(), m_calls.size());
    m_databaseInterface->addCall(callData);

    DialerTypes::CallData data;
    data.id = QString::number(databaseLastId);

    m_calls.push_front(callData); // insert latest calls at the top of the list

    endInsertRows();
}

void CallHistoryModel::clear()
{
    auto reply = m_databaseInterface->clear();
    reply.waitForFinished();
    if (!reply.isValid()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        return;
    }
    beginResetModel();
    m_calls.clear();
    endResetModel();
}

bool CallHistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count)
    auto reply = m_databaseInterface->remove(m_calls[row].id);
    reply.waitForFinished();
    if (!reply.isValid()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        return false;
    }

    beginRemoveRows(parent, row, row);
    fetchCalls();
    endRemoveRows();

    return true;
}

void CallHistoryModel::fetchCalls()
{
    QDBusPendingReply<DialerTypes::CallDataVector> reply = m_databaseInterface->fetchCalls();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    m_calls = reply;
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
        return m_calls[row].id;
    case Roles::ProtocolRole:
        return m_calls[row].protocol;
    case Roles::AccountRole:
        return m_calls[row].account;
    case Roles::ProviderRole:
        return m_calls[row].provider;
    case Roles::CommunicationWithRole:
        return m_calls[row].communicationWith;
    case Roles::DirectionRole:
        return QVariant::fromValue(m_calls[row].direction);
    case Roles::StateRole:
        return QVariant::fromValue(m_calls[row].state);
    case Roles::StateReasonRole:
        return QVariant::fromValue(m_calls[row].stateReason);
    case Roles::CallAttemptDurationRole:
        return m_calls[row].callAttemptDuration;
    case Roles::StartedAtRole:
        return m_calls[row].startedAt;
    case Roles::DurationRole:
        return m_calls[row].duration;
    }
    return {};
}

int CallHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_calls.size();
}
