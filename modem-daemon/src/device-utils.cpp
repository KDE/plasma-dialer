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
    return m_equipmentIdentifiers;
}

QStringList DeviceUtils::deviceUniList()
{
    return m_deviceUniList;
}

QString DeviceUtils::countryCode()
{
    return m_countryCode;
}

void DeviceUtils::setEquipmentIdentifiers(const QStringList &equipmentIdentifiers)
{
    if (m_equipmentIdentifiers != equipmentIdentifiers) {
        m_equipmentIdentifiers = equipmentIdentifiers;
        Q_EMIT equipmentIdentifiersChanged(m_equipmentIdentifiers);
    }
}

void DeviceUtils::setDeviceUniList(const QStringList &deviceUniList)
{
    if (m_deviceUniList != deviceUniList) {
        m_deviceUniList = deviceUniList;
        Q_EMIT deviceUniListChanged(m_deviceUniList);
    }
}

void DeviceUtils::setCountryCode(const QString &countryCode)
{
    if (m_countryCode != countryCode) {
        m_countryCode = countryCode;
        Q_EMIT countryCodeChanged(m_countryCode);
    }
}
