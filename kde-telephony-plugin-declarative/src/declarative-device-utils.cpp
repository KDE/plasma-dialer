// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-device-utils.h"

DeclarativeDeviceUtils::DeclarativeDeviceUtils(QObject *parent)
    : org::kde::telephony::DeviceUtils(QString::fromLatin1(staticInterfaceName()),
                                       QStringLiteral("/org/kde/telephony/DeviceUtils/tel/mm"),
                                       QDBusConnection::sessionBus(),
                                       parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate DeviceUtils interface";
        return;
    }
}

QStringList DeclarativeDeviceUtils::deviceUniList()
{
    QDBusPendingReply<QStringList> reply = org::kde::telephony::DeviceUtils::deviceUniList();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    return reply.value();
}

QStringList DeclarativeDeviceUtils::equipmentIdentifiers()
{
    QDBusPendingReply<QStringList> reply = org::kde::telephony::DeviceUtils::equipmentIdentifiers();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    return reply.value();
}
