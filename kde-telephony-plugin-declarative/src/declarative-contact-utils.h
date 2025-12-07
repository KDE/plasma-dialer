// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <KContacts/PhoneNumber>
#include <KPeople/PersonsModel>
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
    Q_INVOKABLE QString phoneNumberToContactUri(const QString &phoneNumber);
    Q_INVOKABLE QString photoImageProviderUri(const QString &contactUri);
    bool isValid() const;

Q_SIGNALS:
    void isValidChanged();

private:
    QList<KContacts::PhoneNumber> internalPhoneNumbers(const QString &kPeopleUri) const;
    QString numericOnlyString(QString str) const;
};
