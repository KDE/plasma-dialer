/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QApplication>

#include "dialerutils.h"
#include "call-handler.h"
#include "callhistorymodel.h"
#include "version.h"

#include <TelepathyQt/Types>
#include <TelepathyQt/Debug>
#include <TelepathyQt/ClientRegistrar>
#include <TelepathyQt/CallChannel>
#include <TelepathyQt/ChannelClassSpec>
#include <TelepathyQt/ChannelFactory>
#include <TelepathyQt/Account>
#include <TelepathyQt/AccountSet>
#include <TelepathyQt/AccountManager>
#include <TelepathyQt/PendingReady>

#include <KLocalizedString>
#include <KLocalizedContext>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QtQml>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <KAboutData>
#include <KDBusService>

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

//     app.setQuitOnLastWindowClosed(false);
    QCoreApplication::setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    KLocalizedString::setApplicationDomain("plasma-dialer");

    KDBusService service(KDBusService::Unique);

    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(i18n("Plasma Phone Dialer"));

    QCommandLineOption daemonOption(QStringList() << QStringLiteral("d") <<
                                 QStringLiteral("daemon"),
                                 i18n("Daemon mode. run without displaying anything."));

    parser.addPositionalArgument(QStringLiteral("number"), i18n("Call the given number"));

    parser.addOption(daemonOption);

    parser.process(app);

    qmlRegisterType<CallHistoryModel>("org.kde.phone.dialer", 1, 0, "CallHistoryModel");

    Tp::registerTypes();

    Tp::AccountFactoryPtr accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                      Tp::Features() << Tp::Account::FeatureCore
    );

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                                              << Tp::Connection::FeatureSelfContact
                                                                                              << Tp::Connection::FeatureConnected
    );

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());
    channelFactory->addCommonFeatures(Tp::Channel::FeatureCore);
    channelFactory->addFeaturesForCalls(Tp::Features() << Tp::CallChannel::FeatureContents
                                                       << Tp::CallChannel::FeatureCallState
                                                       << Tp::CallChannel::FeatureCallMembers
                                                       << Tp::CallChannel::FeatureLocalHoldState
    );

//     channelFactory->addFeaturesForTextChats(Tp::Features() << Tp::TextChannel::FeatureMessageQueue
//                                                            << Tp::TextChannel::FeatureMessageSentSignal
//                                                            << Tp::TextChannel::FeatureChatState
//                                                            << Tp::TextChannel::FeatureMessageCapabilities);

    Tp::ContactFactoryPtr contactFactory = Tp::ContactFactory::create(Tp::Features() << Tp::Contact::FeatureAlias
                                                                                     << Tp::Contact::FeatureAvatarData
    );

    Tp::ClientRegistrarPtr registrar = Tp::ClientRegistrar::create(accountFactory, connectionFactory,
                                                                   channelFactory, contactFactory);
    QEventLoop loop;
    Tp::AccountManagerPtr manager = Tp::AccountManager::create();
    Tp::PendingReady *ready = manager->becomeReady();
    QObject::connect(ready, &Tp::PendingReady::finished, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    Tp::AccountPtr simAccount;
    const Tp::AccountSetPtr accountSet = manager->validAccounts();
    const auto accounts = accountSet->accounts();
    for (const Tp::AccountPtr &account : accounts) {
        static const QStringList supportedProtocols = {
            QLatin1String("ofono"),
            QLatin1String("tel"),
        };
        if (supportedProtocols.contains(account->protocolName())) {
            simAccount = account;
            break;
        }
    }

    if (simAccount.isNull()) {
        qCritical() << "Unable to get SIM account";
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    auto *dialerUtils = new DialerUtils(simAccount, &engine);

    qmlRegisterSingletonInstance<DialerUtils>("org.kde.phone.dialer", 1, 0, "DialerUtils", dialerUtils);
    qmlRegisterAnonymousType<QAbstractItemModel>("org.kde.phone.dialer", 1);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Tp::SharedPtr<CallHandler> callHandler(new CallHandler(dialerUtils));
    registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(callHandler), QStringLiteral("Plasma.Dialer"));

    KAboutData aboutData(QStringLiteral("dialer"), i18n("Dialer"), QStringLiteral(PLASMADIALER_VERSION_STRING), i18n("Plasma phone dialer"), KAboutLicense::GPL);
    aboutData.setDesktopFileName(QStringLiteral("org.kde.phone.dialer"));

    KAboutData::setApplicationData(aboutData);

    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().at(0));

    Q_ASSERT(window);

    QObject::connect(&service, &KDBusService::activateRequested, window, [=](const QStringList &arguments, const QString &workingDirectory) {
        Q_UNUSED(workingDirectory);
        window->show();
        window->requestActivate();
        if (arguments.length() > 0) {
            QString numberArg = arguments[1];
            if (numberArg.startsWith(QStringLiteral("tel:"))) {
                numberArg = numberArg.mid(4);
            }
            dialerUtils->dial(numberArg);
        }
    });
    if (!parser.isSet(daemonOption)) {
        window->show();
        window->requestActivate();
    }
    if (!parser.positionalArguments().isEmpty()) {
        QString numberArg = parser.positionalArguments().constFirst();
        if (numberArg.startsWith(QStringLiteral("tel:"))) {
            numberArg = numberArg.mid(4);
        }
        qWarning() << "Calling" << numberArg;
        dialerUtils->dial(numberArg);
    }

    return app.exec();
}
