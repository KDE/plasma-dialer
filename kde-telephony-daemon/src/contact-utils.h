// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QPixmap>

class ContactUtils : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.ContactUtils")

public:
    explicit ContactUtils(QObject *parent = nullptr);

public Q_SLOTS:
    QString displayString(const QString &contact);
    QString imageSource(const QString &contact);
    void changeCountryCode(const QString &countryCode);
};
