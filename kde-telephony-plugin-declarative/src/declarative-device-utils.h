// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QStringList>

#include "deviceutilsinterface.h"

class DeclarativeDeviceUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList deviceUniList READ deviceUniList WRITE setDeviceUniList NOTIFY deviceUniListChanged)
    Q_PROPERTY(QStringList equipmentIdentifiers READ equipmentIdentifiers)

public:
    DeclarativeDeviceUtils(QObject *parent = nullptr);
    void setDeviceUtils(org::kde::telephony::DeviceUtils *deviceUtils);

    QStringList deviceUniList();
    QStringList equipmentIdentifiers();

    void setDeviceUniList(const QStringList &newDeviceUniList);

Q_SIGNALS:
    void deviceUniListChanged();

private:
    org::kde::telephony::DeviceUtils *_deviceUtils;
};
