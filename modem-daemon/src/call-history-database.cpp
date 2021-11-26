// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "call-history-database.h"

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

CallHistoryDatabase::CallHistoryDatabase(QObject *parent)
    : QObject(parent)
    , _database(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("calls")))
{
    const QString databaseLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/plasmaphonedialer");
    if (!QDir().mkpath(databaseLocation)) {
        qDebug() << "Could not create the database directory at" << databaseLocation;
    }

    _database.setDatabaseName(databaseLocation + QStringLiteral("/calls.sqlite"));
    const bool open = _database.open();

    if (!open) {
        qWarning() << "Could not open call database" << _database.lastError();
    }

    QSqlQuery createTable(_database);
    createTable.exec(
        QStringLiteral("CREATE TABLE IF NOT EXISTS "
                       "History( "
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "protocol TEXT, "
                       "account TEXT, "
                       "provider TEXT, "
                       "communicationWith TEXT, "
                       "direction INTEGER, "
                       "state INTEGER, "
                       "stateReason INTEGER, "
                       "callAttemptDuration INTEGER, "
                       "startedAt DATETIME, "
                       "duration INTEGER "
                       ")"));
}

DialerTypes::CallDataVector CallHistoryDatabase::fetchCalls()
{
    QSqlQuery fetchCalls(_database);
    fetchCalls.exec(
        QStringLiteral("SELECT "
                       "id, protocol, account, provider, "
                       "communicationWith, direction, state, stateReason, "
                       "callAttemptDuration, startedAt, duration "
                       "FROM History ORDER BY startedAt DESC"));

    DialerTypes::CallDataVector calls;
    while (fetchCalls.next()) {
        DialerTypes::CallData call;
        call.id = fetchCalls.value(0).toString();
        call.protocol = fetchCalls.value(1).toString();
        call.account = fetchCalls.value(2).toString();
        call.provider = fetchCalls.value(3).toString();

        call.communicationWith = fetchCalls.value(4).toString();
        call.direction = fetchCalls.value(5).value<DialerTypes::CallDirection>();
        call.state = fetchCalls.value(6).value<DialerTypes::CallState>();
        call.stateReason = fetchCalls.value(7).value<DialerTypes::CallStateReason>();

        call.callAttemptDuration = fetchCalls.value(8).toInt();
        call.startedAt = QDateTime::fromString(fetchCalls.value(9).toString(), QStringLiteral("yyyy-MM-ddThh:mm:ss.zzz"));
        call.duration = fetchCalls.value(10).toInt();
        calls.append(call);
    }
    return calls;
}

void CallHistoryDatabase::addCall(const DialerTypes::CallData &callData)
{
    QSqlQuery putCall(_database);
    putCall.prepare(
        QStringLiteral("INSERT INTO History "
                       "("
                       "protocol, account, provider, "
                       "communicationWith, direction, state, stateReason, "
                       "callAttemptDuration, startedAt, duration "
                       ") "
                       "VALUES ("
                       ":protocol, :account, :provider, "
                       ":communicationWith, :direction, :state, :stateReason, "
                       ":callAttemptDuration, :startedAt, :duration "
                       ")"));
    putCall.bindValue(QStringLiteral(":protocol"), callData.protocol);
    putCall.bindValue(QStringLiteral(":account"), callData.account);
    putCall.bindValue(QStringLiteral(":provider"), callData.provider);

    putCall.bindValue(QStringLiteral(":communicationWith"), callData.communicationWith);
    putCall.bindValue(QStringLiteral(":direction"), static_cast<int>(callData.direction));
    putCall.bindValue(QStringLiteral(":state"), static_cast<int>(callData.state));
    putCall.bindValue(QStringLiteral(":stateReason"), static_cast<int>(callData.stateReason));

    putCall.bindValue(QStringLiteral(":callAttemptDuration"), callData.callAttemptDuration);
    putCall.bindValue(QStringLiteral(":startedAt"), callData.startedAt);
    putCall.bindValue(QStringLiteral(":duration"), callData.duration);
    putCall.exec();

    Q_EMIT callsChanged();
}

void CallHistoryDatabase::clear()
{
    QSqlQuery clearCalls(_database);
    clearCalls.exec(QStringLiteral("DELETE FROM History"));

    Q_EMIT callsChanged();
}

void CallHistoryDatabase::remove(const QString &id)
{
    QSqlQuery remove(_database);
    remove.prepare(QStringLiteral("DELETE FROM History WHERE id=:id"));
    remove.bindValue(QStringLiteral(":id"), id);
    remove.exec();

    Q_EMIT callsChanged();
}

int CallHistoryDatabase::lastId() const
{
    QSqlQuery fetch(_database);
    fetch.prepare(QStringLiteral("SELECT id FROM History ORDER BY id DESC LIMIT 1"));
    fetch.exec();
    fetch.first();

    return fetch.value(0).toInt();
}
