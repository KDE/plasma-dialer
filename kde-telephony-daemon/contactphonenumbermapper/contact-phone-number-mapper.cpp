// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "contact-phone-number-mapper.h"

#include <KPeopleBackend/AbstractContact>
#include <QDebug>
#include <QThread>

#include "phone-number-utils.h"

ContactPhoneNumberMapper &ContactPhoneNumberMapper::instance(bool testMode)
{
    static ContactPhoneNumberMapper instance(testMode);
    return instance;
}

ContactPhoneNumberMapper::ContactPhoneNumberMapper(bool testMode)
    : QObject()
    , m_model(new KPeople::PersonsModel(this))
{
    m_cellCountry = m_localeCountry;
    if (testMode) {
        // Start right away since we don't wait on the country code
        processRows(0, m_model->rowCount() - 1);
    }

    m_localeCountry = phoneNumberUtils::localeCountryCode();

    // data updates
    // we only care about additional data, not remove one
    connect(m_model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int first, int last) {
        if (m_cellCountry.empty()) {
            // We can't process until we know the cell country info
            return;
        }
        processRows(first, last);
    });
    processRows(0, m_model->rowCount() - 1);
}

void ContactPhoneNumberMapper::changeCountry(const QString &countryCode)
{
    m_cellCountry = countryCode.toStdString();

    m_numberToUri.clear();

    // Once we know the country we can start processing the contacts
    processRows(0, m_model->rowCount() - 1);
}

void ContactPhoneNumberMapper::processRows(const int first, const int last)
{
    QVector<QString> affectedNumbers;
    for (int i = first; i <= last; i++) {
        const auto index = m_model->index(i);

        // Yes, this code has to be illogical. PersonsModel::PersonVCardRole is actually supposed
        // to return an AbstractContact::Ptr, although the name suggests differneltly. Luckily we can get
        // the actual VCard from it.
        const auto phoneNumbers = m_model->data(index, KPeople::PersonsModel::PersonVCardRole)
                                      .value<KPeople::AbstractContact::Ptr>()
                                      ->customProperty(KPeople::AbstractContact::AllPhoneNumbersProperty)
                                      .toStringList();

        const auto personUri = m_model->data(index, KPeople::PersonsModel::PersonUriRole).toString();

        for (const QString &numberString : phoneNumbers) {
            const auto maybeNormalizedNumber = phoneNumberUtils::normalizeNumber(numberString.toStdString(), phoneNumberUtils::International, m_cellCountry);

            if (std::holds_alternative<std::string>(maybeNormalizedNumber)) {
                const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumber);
                m_numberToUri[normalizedNumber] = personUri;
                affectedNumbers.append(QString::fromStdString(normalizedNumber));
            } else {
                continue;
            }

            if (m_cellCountry != m_localeCountry) {
                const auto maybeNormalizedNumberLocale =
                    phoneNumberUtils::normalizeNumber(numberString.toStdString(), phoneNumberUtils::International, m_localeCountry);

                if (std::holds_alternative<std::string>(maybeNormalizedNumberLocale)) {
                    const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumberLocale);
                    m_numberToUri[normalizedNumber] = personUri;
                    affectedNumbers.append(QString::fromStdString(normalizedNumber));
                }
            }
        }
    }

    Q_EMIT contactsChanged(affectedNumbers);
}

QString ContactPhoneNumberMapper::uriForNumber(const QString &phoneNumber) const
{
    const auto maybeNormalizedNumber = phoneNumberUtils::normalizeNumber(phoneNumber.toStdString(), phoneNumberUtils::International, m_cellCountry);

    if (std::holds_alternative<std::string>(maybeNormalizedNumber)) {
        const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumber);
        if (m_numberToUri.contains(normalizedNumber)) {
            return m_numberToUri.at(normalizedNumber);
        }
    } else {
        return QString();
    }

    if (m_cellCountry != m_localeCountry) {
        const auto maybeNormalizedNumberLocale = phoneNumberUtils::normalizeNumber(phoneNumber.toStdString(), phoneNumberUtils::International, m_localeCountry);

        if (std::holds_alternative<std::string>(maybeNormalizedNumberLocale)) {
            const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumberLocale);
            if (m_numberToUri.contains(normalizedNumber)) {
                return m_numberToUri.at(normalizedNumber);
            }
        }
    }

    return QString();
}
