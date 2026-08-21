// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QGuiApplication>

#include "config.h"
#include "version.h"

#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedContext>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <QCommandLineParser>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QWindow>

static QString parseTelArgument(const QString &numberArg)
{
    QString result;
    if (numberArg.startsWith(QStringLiteral("tel:"))) {
        result = numberArg.mid(4);
    }
    if (numberArg.startsWith(QStringLiteral("callto:"))) {
        result = numberArg.mid(7);
    }
    return result;
}

static void inputCallNumber(QWindow *window, const QString &number)
{
    QMetaObject::invokeMethod(window, "call", Q_ARG(QVariant, number));
}

static void raiseWindow(QWindow *window)
{
    window->raise();
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") && QQuickStyle::name().isEmpty()) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    KLocalizedString::setApplicationDomain("plasma-dialer");
    QCoreApplication::setApplicationName(QStringLiteral("dialer"));
    QCoreApplication::setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("dialer")));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("Phone"));

    KAboutData aboutData(QStringLiteral("dialer"),
                         i18n("Phone"),
                         QStringLiteral(PLASMADIALER_VERSION_STRING),
                         i18n("Plasma phone dialer"),
                         KAboutLicense::GPL,
                         i18n("© 2015-2022 KDE Community"));
    aboutData.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=Plasma%20Mobile%20Dialer");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.plasma.dialer"));
    aboutData.addAuthor(i18n("Alexey Andreyev"), QString(), QStringLiteral("aa13q@ya.ru"));
    KAboutData::setApplicationData(aboutData);

    KCrash::initialize();

    QCommandLineParser parser;
    QCommandLineOption telSchemeOption(QStringList() << QStringLiteral("number"),
                                       QCoreApplication::translate("main", "Input a phone number"),
                                       QCoreApplication::translate("main", "phone number"));
    parser.addOption(telSchemeOption);
    parser.process(app);

    QString telNumber = parser.value(telSchemeOption);

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);

    auto config = Config::self();
    engine.rootContext()->setContextProperty(QStringLiteral("DialerAboutData"), QVariant::fromValue(aboutData));
    engine.rootContext()->setContextProperty(QStringLiteral("Config"), QVariant::fromValue(config));

    KDBusService service(KDBusService::Unique, &app);

    engine.loadFromModule(QStringLiteral("org.kde.plasma.dialer"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().at(0));
    Q_ASSERT(window);

    QObject::connect(&service, &KDBusService::activateRequested, window, [&window](const QStringList &arguments) {
        qDebug() << "Window activation requested over DBus";

        for (const auto &arg : arguments) {
            QString numberArg = parseTelArgument(arg);
            if (!numberArg.isEmpty()) {
                inputCallNumber(window, numberArg);
                break;
            }
        }

        raiseWindow(window);
    });

    if (!parser.positionalArguments().isEmpty()) {
        QString numberArg = parseTelArgument(parser.positionalArguments().constFirst());
        if (!numberArg.isEmpty()) {
            telNumber = numberArg;
        }
    }

    if (!telNumber.isEmpty()) {
        inputCallNumber(window, telNumber);
    }

    return app.exec();
}

#include "main.moc"

