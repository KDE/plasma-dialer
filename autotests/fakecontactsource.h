/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef FAKECONTACTSOURCE_H
#define FAKECONTACTSOURCE_H

#include <KPeopleBackend/AllContactsMonitor>
#include <KPeopleBackend/BasePersonsDataSource>

class FakeContactSource : public KPeople::BasePersonsDataSource
{
public:
    FakeContactSource(QObject *parent, const QVariantList &args = QVariantList());
    QString sourcePluginId() const override;

    void remove(const QString &uri);
    void changeProperty(const QString &key, const QVariant &value);

protected:
    KPeople::AllContactsMonitor *createAllContactsMonitor() override;
};

class FakeAllContactsMonitor : public KPeople::AllContactsMonitor
{
    Q_OBJECT
public:
    explicit FakeAllContactsMonitor();
    void changeProperty(const QString &key, const QVariant &value);
    QMap<QString, KPeople::AbstractContact::Ptr> contacts() override;

    void remove(const QString &uri);

private:
    QMap<QString, KPeople::AbstractContact::Ptr> m_contacts;
};

#endif // FAKECONTACTSOURCE_H
