// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-device-utils.h"

DeclarativeDeviceUtils::DeclarativeDeviceUtils(QObject *parent)
    : QObject(parent)
{
}

void DeclarativeDeviceUtils::setDeviceUtils(org::kde::telephony::DeviceUtils *deviceUtils)
{
    if (!deviceUtils) {
        qDebug() << Q_FUNC_INFO << "Could not initiate DeviceUtils interface";
        return;
    }
    _deviceUtils = deviceUtils;
    connect(_deviceUtils, &org::kde::telephony::DeviceUtils::deviceUniListChanged, this, &DeclarativeDeviceUtils::deviceUniListChanged);
}

QStringList DeclarativeDeviceUtils::deviceUniList()
{
    QDBusPendingReply<QStringList> reply;
    if (!_deviceUtils) {
        qDebug() << Q_FUNC_INFO << "DeviceUtils is not initiated";
        return reply.value();
    }
    reply = _deviceUtils->deviceUniList();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    return reply.value();
}

QStringList DeclarativeDeviceUtils::equipmentIdentifiers()
{
    QDBusPendingReply<QStringList> reply;
    if (!_deviceUtils) {
        qDebug() << Q_FUNC_INFO << "DeviceUtils is not initiated";
        return reply.value();
    }
    reply = _deviceUtils->equipmentIdentifiers();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    return reply.value();
}

void DeclarativeDeviceUtils::setDeviceUniList(const QStringList &newDeviceUniList)
{
    if (!_deviceUtils) {
        qDebug() << Q_FUNC_INFO << "DeviceUtils is not initiated";
        return;
    }
    _deviceUtils->setDeviceUniList(newDeviceUniList);
}
