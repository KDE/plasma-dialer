// SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include <QTest>

#include <QTemporaryFile>

#include "contact-phone-number-mapper.h"
#include "fake-contact-source.h"

#include <KPeople/PersonPluginManager>
#include <kpeopleprivate/personmanager_p.h>

class ContactMapperTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        QVERIFY(m_database.open());

        // For this test we assume we are in Sweden and the implicit country code is +46
        QLocale::setDefault(QLocale::Swedish);

        // Called before the first testfunction is executed
        PersonManager::instance(m_database.fileName());
        m_source = new FakeContactSource(nullptr); // don't own. PersonPluginManager removes it on destruction
        QHash<QString, KPeople::BasePersonsDataSource *> sources;
        sources[QStringLiteral("fakesource")] = m_source;
        KPeople::PersonPluginManager::setDataSourcePlugins(sources);
    }

    void testURI_data()
    {
        QTest::addColumn<QString>("number");
        QTest::addColumn<QString>("uri");

        QTest::newRow("nonexistent number") << QStringLiteral("+49123456") << QString();

        QTest::newRow("contact 2, foreign country code") << QStringLiteral("+1 234 567 890") << QStringLiteral("fakesource://contact2");
        QTest::newRow("contact 2, dashes") << QStringLiteral("+1-234-567-890") << QStringLiteral("fakesource://contact2");

        QTest::newRow("contact 3") << QStringLiteral("+46 666 777 999") << QStringLiteral("fakesource://contact3");
        QTest::newRow("contact 3, 00 instead of +") << QStringLiteral("0046 666 777 999") << QStringLiteral("fakesource://contact3");
        QTest::newRow("contact 3, no spaces") << QStringLiteral("+46666777999") << QStringLiteral("fakesource://contact3");
        QTest::newRow("contact 3, dashes") << QStringLiteral("+46-666-777-999") << QStringLiteral("fakesource://contact3");
        QTest::newRow("contact 5, first number") << QStringLiteral("+46111222333") << QStringLiteral("fakesource://contact5");
        QTest::newRow("contact 5, first number, no country code") << QStringLiteral("0111222333") << QStringLiteral("fakesource://contact5");
        QTest::newRow("contact 5, second number") << QStringLiteral("+46333222111") << QStringLiteral("fakesource://contact5");
        QTest::newRow("contact 5, second number, no country code") << QStringLiteral("0333222111") << QStringLiteral("fakesource://contact5");
        QTest::newRow("invalid number") << QStringLiteral("+49yyy123456") << QString();
    }

    void testURI()
    {
        auto &mapper = ContactPhoneNumberMapper::instance();

        QFETCH(QString, number);
        QFETCH(QString, uri);

        QCOMPARE(mapper.uriForNumber(number), uri);
    }

private:
    FakeContactSource *m_source;
    QTemporaryFile m_database;
};

QTEST_GUILESS_MAIN(ContactMapperTest)

#include "contact-mapper-test.moc"
