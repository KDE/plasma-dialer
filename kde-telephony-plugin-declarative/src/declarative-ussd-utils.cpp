// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-ussd-utils.h"

DeclarativeUssdUtils::DeclarativeUssdUtils(QObject *parent)
    : org::kde::telephony::UssdUtils(QString::fromLatin1(staticInterfaceName()),
                                     QStringLiteral("/org/kde/telephony/UssdUtils/tel/mm"),
                                     QDBusConnection::sessionBus(),
                                     parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate UssdUtils interface";
        return;
    }
}

void DeclarativeUssdUtils::initiate(const QString &deviceUni, const QString &command)
{
    QDBusPendingReply<> reply = org::kde::telephony::UssdUtils::initiate(deviceUni, command);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeUssdUtils::respond(const QString &deviceUni, const QString &reply)
{
    QDBusPendingReply<> dbusReply = org::kde::telephony::UssdUtils::respond(deviceUni, reply);
    dbusReply.waitForFinished();
    if (dbusReply.isError()) {
        qDebug() << Q_FUNC_INFO << dbusReply.error();
    }
}

void DeclarativeUssdUtils::cancel(const QString &deviceUni)
{
    QDBusPendingReply<> reply = org::kde::telephony::UssdUtils::cancel(deviceUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}
