// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "contactmapper.h"

#include <KContacts/VCardConverter>
#include <KPeopleBackend/AbstractContact>
#include <QDebug>
#include <QThread>

#include "phonenumbers/phonenumberutil.h"

using namespace ::i18n::phonenumbers;

ContactMapper::ContactMapper(QObject *parent)
    : QObject(parent)
    , m_model(new KPeople::PersonsModel(this))
{

    const QLocale locale;
    const QStringList qcountry = locale.name().split('_');
    const QString &countrycode(qcountry.constLast());
    m_country = countrycode.toStdString();

    // data updates
    // we only care about additional data, not remove one
    connect(m_model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int first, int last) {
        processRows(first, last);
    });
}

std::string ContactMapper::normalizeNumber(const std::string &numberString) const
{
    PhoneNumber phoneNumber;
    auto error = PhoneNumberUtil::GetInstance()->Parse(numberString, m_country, &phoneNumber);

    if (error != PhoneNumberUtil::NO_PARSING_ERROR) {
        qDebug() << "Error parsing number" << QString::fromStdString(numberString) << error;
        return {};
    }

    std::string formattedNumber;
    PhoneNumberUtil::GetInstance()->Format(phoneNumber, PhoneNumberUtil::INTERNATIONAL, &formattedNumber);
    return formattedNumber;
}

void ContactMapper::processRows(const int first, const int last)
{
    QVector<QString> affectedNumbers;
    for (int i = first; i <= last; i++) {
        const auto index = m_model->index(i);

        // Yes, this code has to be illogical. PersonsModel::PersonVCardRole is actually supposed
        // to return an AbstractContact::Ptr, although the name suggests differneltly. Luckily we can get
        // the actual VCard from it.
        const auto phoneNumbers = m_model->data(index, KPeople::PersonsModel::PersonVCardRole)
                        .value<KPeople::AbstractContact::Ptr>()
                        ->customProperty(KPeople::AbstractContact::AllPhoneNumbersProperty).toStringList();

        const auto personUri = m_model->data(index, KPeople::PersonsModel::PersonUriRole).toString();

        for (const QString &numberString : phoneNumbers) {
            const std::string normalizedNumber = normalizeNumber(numberString.toStdString());
            m_numberToUri[normalizedNumber] = personUri;
            affectedNumbers.append(QString::fromStdString(normalizedNumber));
        }
    }

    emit contactsChanged(affectedNumbers);
}

void ContactMapper::performInitialScan()
{
    processRows(0, m_model->rowCount() - 1);
}

QString ContactMapper::uriForNumber(const QString &phoneNumber) const
{
    const std::string normalizedNumber = normalizeNumber(phoneNumber.toStdString());
    if (m_numberToUri.contains(normalizedNumber)) {
        return m_numberToUri.at(normalizedNumber);
    }
    return QString();
}
