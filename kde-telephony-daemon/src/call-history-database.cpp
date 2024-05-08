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

constexpr auto DATABASE_REVISION = 2; // Keep MIGRATE_TO_LATEST_FROM in sync
#define MIGRATE_TO(n, current)                                                                                                                                 \
    if (current < n) {                                                                                                                                         \
        qDebug() << "Running migration" << #n;                                                                                                                 \
        migrationV##n(current);                                                                                                                                \
    }
#define MIGRATE_TO_LATEST_FROM(current) MIGRATE_TO(2, current)

CallHistoryDatabase::CallHistoryDatabase(QObject *parent)
    : QObject(parent)
    , m_database(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("calls")))
{
    const QString databaseLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/plasmaphonedialer");
    if (!QDir().mkpath(databaseLocation)) {
        qDebug() << "Could not create the database directory at" << databaseLocation;
    }

    m_database.setDatabaseName(databaseLocation + QStringLiteral("/calls.sqlite"));
    const bool open = m_database.open();

    if (!open) {
        qWarning() << "Could not open call database" << m_database.lastError();
    }

    migrate();
}

void CallHistoryDatabase::exec(QSqlQuery &query)
{
    if (query.lastQuery().isEmpty()) {
        // Sending empty queries doesn't make sense
        Q_UNREACHABLE();
    }
    if (!query.exec()) {
        qWarning() << "Query" << query.lastQuery() << "resulted in" << query.lastError();
    }
}

DialerTypes::CallDataVector CallHistoryDatabase::fetchCalls()
{
    QSqlQuery fetchCalls(m_database);
    fetchCalls.prepare(
        QStringLiteral("SELECT "
                       "id, protocol, account, provider, "
                       "communicationWith, direction, state, stateReason, "
                       "callAttemptDuration, startedAt, duration "
                       "FROM History ORDER BY startedAt DESC"));
    exec(fetchCalls);

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
    QSqlQuery putCall(m_database);
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
    exec(putCall);

    Q_EMIT callsChanged();
}

void CallHistoryDatabase::clear()
{
    QSqlQuery clearCalls(m_database);
    clearCalls.prepare(QStringLiteral("DELETE FROM History"));
    exec(clearCalls);

    Q_EMIT callsChanged();
}

void CallHistoryDatabase::remove(const QString &id)
{
    QSqlQuery remove(m_database);
    remove.prepare(QStringLiteral("DELETE FROM History WHERE id=:id"));
    remove.bindValue(QStringLiteral(":id"), id);
    exec(remove);

    Q_EMIT callsChanged();
}

int CallHistoryDatabase::lastId() const
{
    QSqlQuery fetch(m_database);
    fetch.prepare(QStringLiteral("SELECT id FROM History ORDER BY id DESC LIMIT 1"));
    exec(fetch);
    fetch.first();

    return fetch.value(0).toInt();
}

QString CallHistoryDatabase::lastCall(const QString &number, int direction) const
{
    QSqlQuery fetch(m_database);
    fetch.prepare(QStringLiteral("SELECT startedAt FROM History WHERE communicationWith=:number and direction = :direction ORDER BY startedAt DESC LIMIT 1"));
    fetch.bindValue(QStringLiteral(":number"), number);
    fetch.bindValue(QStringLiteral(":direction"), direction);
    exec(fetch);
    fetch.first();

    return fetch.value(0).toString();
}

uint CallHistoryDatabase::guessPreHistoricRevision()
{
    uint result = 0;
    uint columnCount = 0;
    QSqlQuery tableInfo(m_database);
    tableInfo.prepare(QStringLiteral("PRAGMA table_info('History')")); // http://sqlite.org/pragma.html
    exec(tableInfo);
    while (tableInfo.next()) {
        qDebug() << Q_FUNC_INFO << "found column" << tableInfo.value(1).toString();
        columnCount++;
    }
    if (columnCount == 11) {
        qDebug() << Q_FUNC_INFO << "found pre-historic revision v1";
        result = 1; // the db from testing version with the v1 scheme but without migrationId
    }
    return result;
}

void CallHistoryDatabase::migrationV1(uint current)
{
    if (current < 1) {
        QSqlQuery tempTable(m_database);
        tempTable.prepare(QStringLiteral("CREATE TABLE temp_table AS SELECT * FROM History"));
        exec(tempTable);

        QSqlQuery dropOld(m_database);
        dropOld.prepare(QStringLiteral("DROP TABLE History"));
        exec(dropOld);

        QSqlQuery createNew(m_database);
        createNew.prepare(
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
        exec(createNew);

        QSqlQuery copyTemp(m_database);
        // clang-format off
        copyTemp.prepare(
            QStringLiteral("INSERT INTO History "
                            "("
                             "protocol, account, provider, "
                             "communicationWith, direction, state, stateReason, "
                             "callAttemptDuration, startedAt, duration "
                            ") "
                           "SELECT "
                            "'tel', 'unknown', 'unknown', "
                            "temp_table.number, "
                            "CASE temp_table.callType " // direction
                                "WHEN 0 " // IncomingRejected
                                    "THEN 1 " // Incoming
                                "WHEN 1 " // IncomingAccepted
                                    "THEN 1 " // Incoming
                                "WHEN 2 " // Outgoing
                                    "THEN 2 " // Outgoing
                                "ELSE 0 " // Unknown
                           "END, "
                           "7, " // state Terminated
                           "CASE temp_table.callType " // stateReason
                                "WHEN 0 " // IncomingRejected
                                    "THEN 9 " // Deflected
                                "WHEN 1 " // IncomingAccepted
                                    "THEN 3 " // Accepted
                                "WHEN 2 " // Outgoing
                                    "THEN 4 " // TerminatedReason
                                "ELSE 0 " // Unknown
                                    "END, "
                           "0, temp_table.time, temp_table.duration "
                           "FROM temp_table"));
        // clang-format on
        exec(copyTemp);

        QSqlQuery dropTemp(m_database);
        dropTemp.prepare(QStringLiteral("DROP TABLE temp_table"));
        exec(dropTemp);
    }
}

void CallHistoryDatabase::migrationV2(uint current)
{
    MIGRATE_TO(1, current);
}

void CallHistoryDatabase::migrate()
{
    // Create migration table if necessary
    QSqlQuery createMetadata(m_database);
    createMetadata.prepare(QStringLiteral("CREATE TABLE IF NOT EXISTS Metadata (migrationId INTEGER NOT NULL)"));
    exec(createMetadata);

    // Find out current revision
    QSqlQuery currentRevision(m_database);
    currentRevision.prepare(QStringLiteral("SELECT migrationId FROM Metadata ORDER BY migrationId DESC LIMIT 1"));
    exec(currentRevision);
    currentRevision.first();

    uint revision = 0;
    if (currentRevision.isValid()) {
        revision = currentRevision.value(0).toUInt();
    }

    if (revision == 0) {
        revision = guessPreHistoricRevision();
    }

    qDebug() << "current database revision" << revision;

    // Run migration if necessary
    if (revision >= DATABASE_REVISION) {
        return;
    }

    MIGRATE_TO_LATEST_FROM(revision);

    // Update migration info if necessary
    QSqlQuery update(m_database);
    update.prepare(QStringLiteral("INSERT INTO Metadata (migrationId) VALUES (:migrationId)"));
    update.bindValue(QStringLiteral(":migrationId"), DATABASE_REVISION);
    exec(update);
}
