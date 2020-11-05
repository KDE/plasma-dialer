// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <KPeople/PersonsModel>
#include <QObject>

class ContactMapper : public QObject
{
    Q_OBJECT

public:
    explicit ContactMapper(QObject *parent = nullptr);

    /**
     * @brief Returns the KPeople URI belonging to phone number,
     * provided a contact exists containing the phone number.
     * If that is not the case, an empty string is returned.
     * @param phone number
     * @return the uri belonging to the phone number
     */
    QString uriForNumber(const QString &phoneNumber) const;

signals:
    /**
     * @brief contactsChanged is emitted whenever the ContactMapper has new data,
     * because a contact was added to KPeople
     * @param list of affected numbers
     */
    void contactsChanged(const QVector<QString> phoneNumbers);

private slots:
    void processRows(const int first, const int last);

public slots:
    /**
     * @brief Scans all persons known to kpeople for phone numbers
     * This must be run before anything useful can be done with the ContactMapper
     */
    void performInitialScan();

private:
    KPeople::PersonsModel *m_model;
    QHash<QString, QString> m_numberToUri;
};
