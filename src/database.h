// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QDateTime>
#include <QObject>
#include <QSqlDatabase>

#include "dialerutils.h"

struct CallData {
    QString id;
    QString number;
    QDateTime time;
    int duration;
    DialerUtils::CallType callType;
};

class Database : public QObject
{
    Q_OBJECT

public:

    explicit Database(QObject *parent = nullptr);

    // Calls
    QVector<CallData> fetchCalls();
    void addCall(const QString &number, int duration, DialerUtils::CallType type);
    void clear();
    void remove(const QString &id);
    int lastId() const;

private:
    QSqlDatabase m_database;

signals:
    void callsChanged();
};
