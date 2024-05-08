// SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "fake-contact-source.h"

#include <QDebug>

FakeContactSource::FakeContactSource(QObject *parent, const QVariantList &args)
    : BasePersonsDataSource(parent, args)
{
}

QString FakeContactSource::sourcePluginId() const
{
    return QStringLiteral("fakesource://");
}

KPeople::AllContactsMonitor *FakeContactSource::createAllContactsMonitor()
{
    return new FakeAllContactsMonitor();
}

void FakeContactSource::changeProperty(const QString &key, const QVariant &value)
{
    qobject_cast<FakeAllContactsMonitor *>(allContactsMonitor().data())->changeProperty(key, value);
}

void FakeContactSource::remove(const QString &uri)
{
    qobject_cast<FakeAllContactsMonitor *>(allContactsMonitor().data())->remove(uri);
}

class FakeContact : public KPeople::AbstractContact
{
public:
    FakeContact(const QVariantMap &props)
        : _properties(props)
    {
    }

    QVariant customProperty(const QString &key) const override
    {
        if (key == KPeople::AbstractContact::AllPhoneNumbersProperty) {
            const QStringList allNumbers = _properties[KPeople::AbstractContact::AllPhoneNumbersProperty].toStringList();

            if (allNumbers.isEmpty()) {
                return _properties[KPeople::AbstractContact::PhoneNumberProperty];
            }

            return _properties[KPeople::AbstractContact::AllPhoneNumbersProperty];
        }

        if (key.startsWith(QLatin1String("all-"))) {
            return QStringList(_properties[key.mid(4)].toString());
        } else {
            return _properties[key];
        }
    }

    QVariantMap _properties;
};

FakeAllContactsMonitor::FakeAllContactsMonitor()
{
    // clang-format off
    m_contacts.insert(
        QStringLiteral("fakesource://contact1"),
        KPeople::AbstractContact::Ptr(new FakeContact({
            {KPeople::AbstractContact::NameProperty, QStringLiteral("Contact 1")},
            {KPeople::AbstractContact::EmailProperty, QStringLiteral("contact1@example.com")}
        }))
    );

    m_contacts.insert(
        QStringLiteral("fakesource://contact2"),
        KPeople::AbstractContact::Ptr(new FakeContact({
            {KPeople::AbstractContact::NameProperty, QStringLiteral("Contact 2")},
            {KPeople::AbstractContact::EmailProperty, QStringLiteral("contact2@example.com")},
            {KPeople::AbstractContact::PhoneNumberProperty, QStringLiteral("+1 234 567 890")}
        }))
    );

    m_contacts.insert(
        QStringLiteral("fakesource://contact3"),
        KPeople::AbstractContact::Ptr(new FakeContact({{KPeople::AbstractContact::NameProperty, QStringLiteral("Contact 3")},
            {KPeople::AbstractContact::EmailProperty, QStringLiteral("contact3@example.com")},
            {KPeople::AbstractContact::PhoneNumberProperty, QStringLiteral("+46 666 777 999")}
        }))
    );

    m_contacts.insert(
        QStringLiteral("fakesource://contact4"),
        KPeople::AbstractContact::Ptr(new FakeContact({
            {KPeople::AbstractContact::NameProperty, QStringLiteral("Contact 4")},
            {KPeople::AbstractContact::EmailProperty, QStringLiteral("contact4@example.com")},
            {KPeople::AbstractContact::AllPhoneNumbersProperty, QStringList{QStringLiteral("+46-111-111-111")}}
        }))
    );

    m_contacts.insert(
        QStringLiteral("fakesource://contact5"),
        KPeople::AbstractContact::Ptr(new FakeContact({
            {KPeople::AbstractContact::NameProperty, QStringLiteral("Contact 5")},
            {KPeople::AbstractContact::AllPhoneNumbersProperty, QVariantList{QStringLiteral("+46 111 222 333"), QStringLiteral("0 333 222 111")}}
        }))
    );

    m_contacts.insert(
        QStringLiteral("fakesource://contact6"),
        KPeople::AbstractContact::Ptr(new FakeContact({
            {KPeople::AbstractContact::NameProperty, QStringLiteral("Malory")},
            {KPeople::AbstractContact::AllPhoneNumbersProperty, QVariantList{QStringLiteral("+46 111&222x333") /* Intentionally invalid */}}
        }))
    );

    // clang-format on

    emitInitialFetchComplete(true);
}

void FakeAllContactsMonitor::remove(const QString &uri)
{
    m_contacts.remove(uri);
    Q_EMIT contactRemoved(uri);
}

QMap<QString, KPeople::AbstractContact::Ptr> FakeAllContactsMonitor::contacts()
{
    return m_contacts;
}

void FakeAllContactsMonitor::changeProperty(const QString &key, const QVariant &value)
{
    KPeople::AbstractContact::Ptr contact1 = contacts()[QStringLiteral("fakesource://contact1")];
    static_cast<FakeContact *>(contact1.data())->_properties[key] = value;
    Q_ASSERT(contact1->customProperty(key) == value);

    Q_EMIT contactChanged(QStringLiteral("fakesource://contact1"), contact1);
}
