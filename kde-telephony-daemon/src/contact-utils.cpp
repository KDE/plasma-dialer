// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "contact-utils.h"

#include <KPeople/PersonData>
#include <QSharedPointer>

#include "contact-phone-number-mapper.h"
#include "deviceutilsinterface.h"

static QSharedPointer<KPeople::PersonData> contactData(const QString &contact)
{
    const QString uri = ContactPhoneNumberMapper::instance().uriForNumber(contact);
    return QSharedPointer<KPeople::PersonData>(new KPeople::PersonData(uri));
}

ContactUtils::ContactUtils(QObject *parent)
    : QObject(parent)
{
}

QString ContactUtils::displayString(const QString &contact)
{
    auto person = contactData(contact);
    const QString name = person->name();
    if (!name.isEmpty()) {
        return name;
    }
    return contact;
}

QPixmap ContactUtils::picture(const QString &contact)
{
    auto person = contactData(contact);
    return person->photo();
}

void ContactUtils::changeCountryCode(const QString &countryCode)
{
    ContactPhoneNumberMapper::instance().changeCountry(countryCode);
}
