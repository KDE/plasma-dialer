// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "phone-number-utils.h"

#include <QChar>
#include <QLocale>

#include <phonenumbers/phonenumberutil.h>

using namespace ::i18n::phonenumbers;

namespace phoneNumberUtils
{
const std::string localeCountryCode()
{
    const QLocale locale;
    const QStringList qcountry = locale.name().split(u'_');
    const QString &countrycode(qcountry.constLast());
    return countrycode.toStdString();
}

NormalizeResult normalizeNumber(const std::string &numberString, PhoneNumberFormat format, const std::string &country)
{
    std::string countryToCheck = country;
    if (countryToCheck.empty()) {
        countryToCheck = localeCountryCode();
    }
    PhoneNumber phoneNumber;
    auto error = PhoneNumberUtil::GetInstance()->Parse(numberString, countryToCheck, &phoneNumber);

    if (error != PhoneNumberUtil::NO_PARSING_ERROR) {
        return ErrorType(error);
    }

    std::string formattedNumber;
    PhoneNumberUtil::GetInstance()->Format(phoneNumber, PhoneNumberUtil::PhoneNumberFormat(format), &formattedNumber);
    return formattedNumber;
}

QString normalizeNumber(const QString &numberString, PhoneNumberFormat format, const std::string &country)
{
    auto res = normalizeNumber(numberString.toStdString(), format, country);
    if (std::holds_alternative<std::string>(res)) {
        return QString::fromStdString(std::get<std::string>(res));
    }

    return numberString;
}
}
