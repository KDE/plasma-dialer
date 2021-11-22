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
    _deviceUtils = deviceUtils;
    _modemController = modemController;

    connect(_modemController, &ModemController::deviceUniListChanged, _deviceUtils, &DeviceUtils::setDeviceUniList);
    connect(_modemController, &ModemController::countryCodeChanged, _deviceUtils, &DeviceUtils::setCountryCode);

    _deviceUtils->setDeviceUniList(_modemController->deviceUniList());

    QStringList eqids;
    for (const auto &uni : _modemController->deviceUniList()) {
        eqids << _modemController->equipmentIdentifier(uni);
    }
    _deviceUtils->setEquipmentIdentifiers(eqids);
}
