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
    org::kde::telephony::UssdUtils::initiate(deviceUni, command);
}

void DeclarativeUssdUtils::respond(const QString &deviceUni, const QString &reply)
{
    org::kde::telephony::UssdUtils::respond(deviceUni, reply);
}

void DeclarativeUssdUtils::cancel(const QString &deviceUni)
{
    org::kde::telephony::UssdUtils::cancel(deviceUni);
}
