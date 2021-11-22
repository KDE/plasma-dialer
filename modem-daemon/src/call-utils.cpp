// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "call-utils.h"

#include <QDebug>

#include "phonenumbers/asyoutypeformatter.h"
#include "phonenumbers/phonenumberutil.h"

CallUtils::CallUtils(QObject *parent)
    : QObject(parent)
{
}

CallUtils::~CallUtils() = default;

void CallUtils::dial(const QString &deviceUni, const QString &number)
{
    qDebug() << "Starting a call " << number << " via " << deviceUni;
    Q_EMIT dialed(deviceUni, number);
}

QString CallUtils::formatNumber(const QString &number)
{
    using namespace ::i18n::phonenumbers;

    // Get formatter instance
    QLocale locale;
    QStringList qcountry = locale.name().split(QStringLiteral("_"));
    const QString &countrycode(qcountry.constLast());
    string country = countrycode.toUtf8().constData();
    PhoneNumberUtil *util = PhoneNumberUtil::GetInstance();
    AsYouTypeFormatter *formatter = util->PhoneNumberUtil::GetAsYouTypeFormatter(country);

    // Normalize input
    string stdnumber = number.toUtf8().constData();
    util->NormalizeDiallableCharsOnly(&stdnumber);

    // Format
    string formatted;
    formatter->Clear();
    for (char &c : stdnumber) {
        formatter->InputDigit(c, &formatted);
    }
    delete formatter;

    return QString::fromStdString(formatted);
}

void CallUtils::accept(const QString &deviceUni, const QString &callUni)
{
    Q_EMIT accepted(deviceUni, callUni);
}

void CallUtils::hangUp(const QString &deviceUni, const QString &callUni)
{
    Q_EMIT hungUp(deviceUni, callUni);
}

void CallUtils::sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones)
{
    Q_EMIT sentDtmf(deviceUni, callUni, tones);
}

void CallUtils::fetchCalls()
{
    Q_EMIT fetchedCalls();
}

void CallUtils::setFetchedCalls(const DialerTypes::CallDataVector &fetchedCalls)
{
    Q_EMIT fetchedCallsChanged(fetchedCalls);
}
