// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "device-utils.h"

DeviceUtils::DeviceUtils(QObject *parent)
    : QObject(parent)
{
}

QStringList DeviceUtils::equipmentIdentifiers()
{
    return _equipmentIdentifiers;
}

QStringList DeviceUtils::deviceUniList()
{
    return _deviceUniList;
}

QString DeviceUtils::countryCode()
{
    return _countryCode;
}

void DeviceUtils::setEquipmentIdentifiers(const QStringList &equipmentIdentifiers)
{
    if (_equipmentIdentifiers != equipmentIdentifiers) {
        _equipmentIdentifiers = equipmentIdentifiers;
        Q_EMIT equipmentIdentifiersChanged(_equipmentIdentifiers);
    }
}

void DeviceUtils::setDeviceUniList(const QStringList &deviceUniList)
{
    if (_deviceUniList != deviceUniList) {
        _deviceUniList = deviceUniList;
        Q_EMIT deviceUniListChanged(_deviceUniList);
    }
}

void DeviceUtils::setCountryCode(const QString &countryCode)
{
    if (_countryCode != countryCode) {
        _countryCode = countryCode;
        Q_EMIT countryCodeChanged(_countryCode);
    }
}
