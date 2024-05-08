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
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

public:
    DeclarativeContactUtils(QObject *parent = nullptr);

    Q_INVOKABLE QString displayString(const QString &contact);
    Q_INVOKABLE QVariantList phoneNumbers(const QString &kPeopleUri);
    bool isValid() const;
Q_SIGNALS:
    void isValidChanged();
};
