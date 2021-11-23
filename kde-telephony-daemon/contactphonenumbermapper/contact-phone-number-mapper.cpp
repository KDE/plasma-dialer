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
    , _model(new KPeople::PersonsModel(this))
{
    _cellCountry = _localeCountry;
    if (testMode) {
        // Start right away since we don't wait on the country code
        processRows(0, _model->rowCount() - 1);
    }

    _localeCountry = phoneNumberUtils::localeCountryCode();

    // data updates
    // we only care about additional data, not remove one
    connect(_model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int first, int last) {
        if (_cellCountry.empty()) {
            // We can't process until we know the cell country info
            return;
        }
        processRows(first, last);
    });
    processRows(0, _model->rowCount() - 1);
}

void ContactPhoneNumberMapper::changeCountry(const QString &countryCode)
{
    _cellCountry = countryCode.toStdString();

    _numberToUri.clear();

    // Once we know the country we can start processing the contacts
    processRows(0, _model->rowCount() - 1);
}

void ContactPhoneNumberMapper::processRows(const int first, const int last)
{
    QVector<QString> affectedNumbers;
    for (int i = first; i <= last; i++) {
        const auto index = _model->index(i);

        // Yes, this code has to be illogical. PersonsModel::PersonVCardRole is actually supposed
        // to return an AbstractContact::Ptr, although the name suggests differneltly. Luckily we can get
        // the actual VCard from it.
        const auto phoneNumbers = _model->data(index, KPeople::PersonsModel::PersonVCardRole)
                                      .value<KPeople::AbstractContact::Ptr>()
                                      ->customProperty(KPeople::AbstractContact::AllPhoneNumbersProperty)
                                      .toStringList();

        const auto personUri = _model->data(index, KPeople::PersonsModel::PersonUriRole).toString();

        for (const QString &numberString : phoneNumbers) {
            const auto maybeNormalizedNumber = phoneNumberUtils::normalizeNumber(numberString.toStdString(), phoneNumberUtils::International, _cellCountry);

            if (std::holds_alternative<std::string>(maybeNormalizedNumber)) {
                const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumber);
                _numberToUri[normalizedNumber] = personUri;
                affectedNumbers.append(QString::fromStdString(normalizedNumber));
            } else {
                continue;
            }

            if (_cellCountry != _localeCountry) {
                const auto maybeNormalizedNumberLocale =
                    phoneNumberUtils::normalizeNumber(numberString.toStdString(), phoneNumberUtils::International, _localeCountry);

                if (std::holds_alternative<std::string>(maybeNormalizedNumberLocale)) {
                    const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumberLocale);
                    _numberToUri[normalizedNumber] = personUri;
                    affectedNumbers.append(QString::fromStdString(normalizedNumber));
                }
            }
        }
    }

    Q_EMIT contactsChanged(affectedNumbers);
}

QString ContactPhoneNumberMapper::uriForNumber(const QString &phoneNumber) const
{
    const auto maybeNormalizedNumber = phoneNumberUtils::normalizeNumber(phoneNumber.toStdString(), phoneNumberUtils::International, _cellCountry);

    if (std::holds_alternative<std::string>(maybeNormalizedNumber)) {
        const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumber);
        if (_numberToUri.contains(normalizedNumber)) {
            return _numberToUri.at(normalizedNumber);
        }
    } else {
        return QString();
    }

    if (_cellCountry != _localeCountry) {
        const auto maybeNormalizedNumberLocale = phoneNumberUtils::normalizeNumber(phoneNumber.toStdString(), phoneNumberUtils::International, _localeCountry);

        if (std::holds_alternative<std::string>(maybeNormalizedNumberLocale)) {
            const auto &normalizedNumber = std::get<std::string>(maybeNormalizedNumberLocale);
            if (_numberToUri.contains(normalizedNumber)) {
                return _numberToUri.at(normalizedNumber);
            }
        }
    }

    return QString();
}
