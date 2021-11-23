// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

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

#pragma once

#include <KPeople/PersonsModel>
#include <QObject>

#include <unordered_map>

class ContactPhoneNumberMapper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns the KPeople URI belonging to phone number,
     * provided a contact exists containing the phone number.
     * If that is not the case, an empty string is returned.
     * @param phone number
     * @param guess the country
     * if a contact's phonenumber in the address book
     * is saved without a country code
     * @return the uri belonging to the phone number
     */
    QString uriForNumber(const QString &phoneNumber) const;

    static ContactPhoneNumberMapper &instance(bool testMode = false);

Q_SIGNALS:
    /**
     * @brief contactsChanged is emitted whenever the ContactMapper has new data,
     * because a contact was added to KPeople
     * @param list of affected numbers
     */
    void contactsChanged(const QVector<QString> phoneNumbers);

public Q_SLOTS:
    /**
     * @brief changeCountry should be executed by the daemon via appropriate signal
     * @param new two-letter country code
     */
    void changeCountry(const QString &countryCode);

private Q_SLOTS:
    void processRows(const int first, const int last);

private:
    explicit ContactPhoneNumberMapper(bool testMode);
    [[nodiscard]] std::string normalizeNumber(const std::string &numberString) const;

    KPeople::PersonsModel *_model;
    std::unordered_map<std::string, QString> _numberToUri;

    std::string _cellCountry;
    std::string _localeCountry;
};
