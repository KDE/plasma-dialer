// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "contactmapper.h"

#include <KContacts/VCardConverter>
#include <KPeopleBackend/AbstractContact>
#include <QDebug>
#include <QThread>

#include "phonenumbers/phonenumberutil.h"

#include <qofonomanager.h>

using namespace ::i18n::phonenumbers;

/*
 * This class tries to match a phonenumber to one of your contacts.
 * This is not an easy tasks and involves some heuristics and tradeoffs.
 *
 * It works by maintaining a mapping from phonenumbers to KPeople URIs.
 * Since phonenumbers can be written in different formats the numbers are
 * normalized to a common form before comparing.
 *
 * If a contact's phonenumber in the address book is saved without a country
 * code then we need to guess a country. Guessing the country based on the locale
 * is problematic for people that have e.g. their phone in English but live
 * in Germany and assume the German country code in their address book.
 * Querying the country based on the cell network/MCC is problematic when travelling
 * abroad and the MCC changes.
 *
 * Our heuristic works like this:
 * First we normalize the number based on the cell network country and store the mapping.
 * If the locale's country is different to the cell network country we normalize the number
 * based on that and add it to the mapping as well.
 * When a call is received we first normalize the number based on the cell network country and
 * ask the mapping. If there is no entry we do the same based on the locale country.
 *
 * This covers the case of someone from Germany with English locale and the case of someone
 * from Germany using German locale travelling to France. It does not cover the case of
 * someone from Germany using English locale travelling to France.
 *
 * When the MCC changes, i.e. when we cross the border the mapping is cleared and redone with the new MCC.
 */

ContactMapper &ContactMapper::instance(bool testMode)
{
    static ContactMapper instance(testMode);
    return instance;
}

ContactMapper::ContactMapper(bool testMode)
    : QObject()
    , m_model(new KPeople::PersonsModel(this))
{
    if (!testMode) {
        QOfonoManager manager;
        manager.getModems();
        QString modemPath = manager.defaultModem();
        if (modemPath.isEmpty()) {
            qWarning() << "Could not get default modem path";
        }

        m_networkRegistration = std::make_unique<QOfonoNetworkRegistration>();
        m_networkRegistration->setModemPath(modemPath);

        connect(m_networkRegistration.get(), &QOfonoNetworkRegistration::countryChanged, this, [this] {
            const QString country = m_networkRegistration->country();

            m_cellCountry = country.toStdString();

            m_numberToUri.clear();

            // Once we know the country we can start processing the contacts
            processRows(0, m_model->rowCount() - 1);
        });
    } else {
        // Use a hardcoded country when running tests instead querying a potentially nonexistant modem
        m_cellCountry = "SE";

        // Start right away since we don't wait on the country code
        processRows(0, m_model->rowCount() - 1);
    }

    const QLocale locale;
    const QStringList qcountry = locale.name().split('_');
    const QString &countrycode(qcountry.constLast());
    m_localeCountry = countrycode.toStdString();

    // data updates
    // we only care about additional data, not remove one
    connect(m_model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int first, int last) {
        if (m_cellCountry.empty()) {
            // We can't process until we know the cell country info
            return;
        }
        processRows(first, last);
    });
}

std::optional<std::string> ContactMapper::normalizeNumber(const std::string &numberString, const std::string &country) const
{
    PhoneNumber phoneNumber;
    auto error = PhoneNumberUtil::GetInstance()->Parse(numberString, country, &phoneNumber);

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
                                      ->customProperty(KPeople::AbstractContact::AllPhoneNumbersProperty)
                                      .toStringList();

        const auto personUri = m_model->data(index, KPeople::PersonsModel::PersonUriRole).toString();

        for (const QString &numberString : phoneNumbers) {
            const std::optional<std::string> maybeNormalizedNumber = normalizeNumber(numberString.toStdString(), m_cellCountry);

            if (!maybeNormalizedNumber) {
                continue;
            }

            m_numberToUri[*maybeNormalizedNumber] = personUri;
            affectedNumbers.append(QString::fromStdString(*maybeNormalizedNumber));

            if (m_cellCountry != m_localeCountry) {
                const std::optional<std::string> maybeNormalizedNumberLocale = normalizeNumber(numberString.toStdString(), m_localeCountry);

                if (maybeNormalizedNumberLocale) {
                    m_numberToUri[*maybeNormalizedNumberLocale] = personUri;
                    affectedNumbers.append(QString::fromStdString(*maybeNormalizedNumberLocale));
                }
            }
        }
    }

    emit contactsChanged(affectedNumbers);
}

QString ContactMapper::uriForNumber(const QString &phoneNumber) const
{
    const std::optional<std::string> maybeNormalizedNumber = normalizeNumber(phoneNumber.toStdString(), m_cellCountry);

    if (!maybeNormalizedNumber) {
        return QString();
    }

    if (m_numberToUri.contains(*maybeNormalizedNumber)) {
        return m_numberToUri.at(*maybeNormalizedNumber);
    }

    if (m_cellCountry != m_localeCountry) {
        const std::optional<std::string> maybeNormalizedNumberLocale = normalizeNumber(phoneNumber.toStdString(), m_localeCountry);
        if (maybeNormalizedNumberLocale && m_numberToUri.contains(*maybeNormalizedNumberLocale)) {
            return m_numberToUri.at(*maybeNormalizedNumberLocale);
        }
    }

    return QString();
}
