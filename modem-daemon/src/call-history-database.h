// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QSqlDatabase>

#include <kTelephonyMetaTypes/dialer-types.h>

class CallHistoryDatabase : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.CallHistoryDatabase")

public:
    explicit CallHistoryDatabase(QObject *parent = nullptr);

Q_SIGNALS:
    void callsChanged();

public Q_SLOTS:
    DialerTypes::CallDataVector fetchCalls();
    void addCall(const DialerTypes::CallData &callData);
    void clear();
    void remove(const QString &id);
    int lastId() const;

private:
    QSqlDatabase _database;
};
