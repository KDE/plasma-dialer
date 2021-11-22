// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

class ModemController;

class DeviceUtils : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.DeviceUtils")

public:
    explicit DeviceUtils(QObject *parent = nullptr);

    Q_INVOKABLE QStringList equipmentIdentifiers();
    Q_INVOKABLE QStringList deviceUniList();
    Q_INVOKABLE QString countryCode();

    Q_INVOKABLE void setEquipmentIdentifiers(const QStringList &equipmentIdentifiers);
    Q_INVOKABLE void setDeviceUniList(const QStringList &deviceUniList);
    Q_INVOKABLE void setCountryCode(const QString &countryCode);

Q_SIGNALS:
    void equipmentIdentifiersChanged(const QStringList &equipmentIdentifiers);
    void deviceUniListChanged(const QStringList &deviceUniList);
    void countryCodeChanged(const QString &countryCode);

private:
    QStringList _deviceUniList;
    QStringList _equipmentIdentifiers;
    QString _countryCode;
};
