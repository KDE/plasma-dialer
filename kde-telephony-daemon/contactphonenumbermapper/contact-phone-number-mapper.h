// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <KPeople/PersonsModel>
#include <QObject>

#include <qofononetworkregistration.h>

#include <optional>

class ContactMapper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns the KPeople URI belonging to phone number,
     * provided a contact exists containing the phone number.
     * If that is not the case, an empty string is returned.
     * @param phone number
     * @return the uri belonging to the phone number
     */
    QString uriForNumber(const QString &phoneNumber) const;

    static ContactMapper &instance(bool testMode = false);

signals:
    /**
     * @brief contactsChanged is emitted whenever the ContactMapper has new data,
     * because a contact was added to KPeople
     * @param list of affected numbers
     */
    void contactsChanged(const QVector<QString> phoneNumbers);

private slots:
    void processRows(const int first, const int last);

private:
    explicit ContactMapper(bool testMode);
    [[nodiscard]] std::optional<std::string> normalizeNumber(const std::string &numberString, const std::string &country) const;

    KPeople::PersonsModel *m_model;
    std::unordered_map<std::string, QString> m_numberToUri;
    std::string m_cellCountry;
    std::string m_localeCountry;
    std::unique_ptr<QOfonoNetworkRegistration> m_networkRegistration;
};
