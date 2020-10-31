/*
 *   Copyright 2020 Bhushan Shah <bshah@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
