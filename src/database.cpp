// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "database.h"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

Database::Database(QObject *parent)
    : QObject(parent)
    , m_database(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("calls")))
{
    const auto databaseLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/plasmaphonedialer");
    if (!QDir().mkpath(databaseLocation)) {
        qDebug() << "Could not create the database directory at" << databaseLocation;
    }

    m_database.setDatabaseName(databaseLocation + QStringLiteral("/calls.sqlite"));
    const bool open = m_database.open();

    if (!open) {
        qWarning() << "Could not open call database" << m_database.lastError();
    }

    QSqlQuery createTable(m_database);
    createTable.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS History(id INTEGER PRIMARY KEY AUTOINCREMENT, number TEXT, time DATETIME, duration INTEGER, callType INTEGER)"));
}

QVector<CallData> Database::fetchCalls()
{
    QSqlQuery fetchCalls(m_database);
    fetchCalls.exec(QStringLiteral("SELECT id, number, time, duration, callType FROM History ORDER BY time DESC"));

    QVector<CallData> calls;
    while (fetchCalls.next()) {
        CallData call;
        call.id = fetchCalls.value(0).toString();
        call.number = fetchCalls.value(1).toString();
        call.time = QDateTime::fromMSecsSinceEpoch(fetchCalls.value(2).toInt());
        call.duration = fetchCalls.value(3).toInt();
        call.callType = fetchCalls.value(4).value<DialerUtils::CallType>();

        calls.append(call);
    }
    return calls;
}

void Database::addCall(const QString &number, int duration, DialerUtils::CallType type)
{
    QSqlQuery putCall(m_database);
    putCall.prepare(QStringLiteral("INSERT INTO History (number, time, duration, callType) VALUES (:number, :time, :duration, :callType)"));
    putCall.bindValue(QStringLiteral(":number"), number);
    putCall.bindValue(QStringLiteral(":time"), QDateTime::currentDateTime().toMSecsSinceEpoch());
    putCall.bindValue(QStringLiteral(":duration"), duration);
    putCall.bindValue(QStringLiteral(":callType"), type);
    putCall.exec();

    emit callsChanged();
}

void Database::clear()
{
    QSqlQuery clearCalls(m_database);
    clearCalls.exec(QStringLiteral("DELETE FROM History"));

    emit callsChanged();
}

void Database::remove(const QString &id)
{
    QSqlQuery remove(m_database);
    remove.prepare(QStringLiteral("DELETE FROM History WHERE id=:id"));
    remove.bindValue(QStringLiteral(":id"), id);
    remove.exec();

    emit callsChanged();
}

int Database::lastId() const
{
    QSqlQuery fetch(m_database);
    fetch.prepare(QStringLiteral("SELECT id FROM History ORDER BY id DESC LIMIT 1"));
    fetch.exec();
    fetch.first();

    return fetch.value(0).toInt();
}
