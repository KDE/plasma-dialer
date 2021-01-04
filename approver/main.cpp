/*
 *   SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QCommandLineParser>
#include <QCoreApplication>

#include <KDBusService>
#include <KLocalizedString>

#include <TelepathyQt/Types>
#include <TelepathyQt/Debug>
#include <TelepathyQt/Channel>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/ChannelClassSpec>
#include <TelepathyQt/ClientRegistrar>
#include <TelepathyQt/CallChannel>

#include "phoneapprover.h"
#include "version.h"

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    QCoreApplication app(argc, argv);

    app.setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("kde.org"));

    KLocalizedString::setApplicationDomain("plasma-dialer");

    KDBusService service(KDBusService::Unique);
    Tp::registerTypes();
    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    Tp::AccountFactoryPtr accountFactory =
        Tp::AccountFactory::create(QDBusConnection::sessionBus());
    Tp::ConnectionFactoryPtr connectionFactory =
        Tp::ConnectionFactory::create(QDBusConnection::sessionBus());

    Tp::ChannelFactoryPtr channelFactory =
        Tp::ChannelFactory::create(QDBusConnection::sessionBus());
        
    channelFactory->addCommonFeatures(Tp::Channel::FeatureCore);
    Tp::ContactFactoryPtr contactFactory =
    Tp::ContactFactory::create(Tp::Features()
        << Tp::Contact::FeatureAlias
        << Tp::Contact::FeatureAvatarData);

    Tp::ClientRegistrarPtr m_registrar;
    m_registrar = Tp::ClientRegistrar::create(accountFactory, connectionFactory,
                                              channelFactory, contactFactory);
    m_registrar->registerClient(Tp::SharedPtr<PhoneApprover>(new PhoneApprover()),
                                QStringLiteral("Plasma.Approver"));
    return app.exec();
}
