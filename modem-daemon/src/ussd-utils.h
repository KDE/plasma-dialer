// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

class UssdUtils : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.UssdUtils")

public:
    explicit UssdUtils(QObject *parent = nullptr);

public Q_SLOTS:
    void initiate(const QString &deviceUni, const QString &command);
    void respond(const QString &deviceUni, const QString &reply);
    void cancel(const QString &deviceUni);

Q_SIGNALS:
    void initiated(const QString &deviceUni, const QString &command);
    void responded(const QString &deviceUni, const QString &reply);
    void cancelled(const QString &deviceUni);
    void notificationReceived(const QString &deviceUni, const QString &message);
    void requestReceived(const QString &deviceUni, const QString &message);
};
