// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <KContacts/PhoneNumber>
#include <QObject>

#include "contactutilsinterface.h"

class DeclarativeContactUtils : public org::kde::telephony::ContactUtils
{
    Q_OBJECT
public:
    DeclarativeContactUtils(QObject *parent = nullptr);

    Q_INVOKABLE QString displayString(const QString &contact);
    Q_INVOKABLE QVariantList phoneNumbers(const QString &kPeopleUri);
};
