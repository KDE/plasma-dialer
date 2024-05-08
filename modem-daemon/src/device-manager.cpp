// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "device-manager.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>

#include "device-utils.h"
#include "modem-controller.h"

DeviceManager::DeviceManager(ModemController *modemController, DeviceUtils *deviceUtils, QObject *parent)
    : QObject(parent)
{
    m_deviceUtils = deviceUtils;
    m_modemController = modemController;

    connect(m_modemController, &ModemController::deviceUniListChanged, m_deviceUtils, &DeviceUtils::setDeviceUniList);
    connect(m_modemController, &ModemController::countryCodeChanged, m_deviceUtils, &DeviceUtils::setCountryCode);

    m_deviceUtils->setDeviceUniList(m_modemController->deviceUniList());

    QStringList eqids;
    for (const auto &uni : m_modemController->deviceUniList()) {
        eqids << m_modemController->equipmentIdentifier(uni);
    }
    m_deviceUtils->setEquipmentIdentifiers(eqids);
}
