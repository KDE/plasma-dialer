// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include "deviceutilsinterface.h"

class DeclarativeDeviceUtils : public org::kde::telephony::DeviceUtils
{
    Q_OBJECT
public:
    DeclarativeDeviceUtils(QObject *parent = nullptr);

    Q_INVOKABLE QStringList deviceUniList();
    Q_INVOKABLE QStringList equipmentIdentifiers();
};
