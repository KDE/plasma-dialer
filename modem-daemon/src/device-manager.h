// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

class ModemController;
class DeviceUtils;

class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(ModemController *modemController, DeviceUtils *deviceUtils, QObject *parent = nullptr);

private:
    DeviceUtils *_deviceUtils;
    ModemController *_modemController;
};
