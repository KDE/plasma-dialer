// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QGuiApplication>

#include "version.h"

#include <KDBusService>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KWindowSystem>
#include <QCommandLineParser>
#include <QIcon>
#include <QObject>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QtQml>

#include <KAboutData>

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

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
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

    KAboutData aboutData(QStringLiteral("dialer"), i18n("Phone"), QStringLiteral(PLASMADIALER_VERSION_STRING), i18n("Plasma phone dialer"), KAboutLicense::GPL);
    aboutData.setDesktopFileName(QStringLiteral("org.kde.phone.dialer"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    QCommandLineOption telSchemeOption(QStringList() << QStringLiteral("number"),
                                       QCoreApplication::translate("main", "Input a phone number"),
                                       QCoreApplication::translate("main", "phone number"));
    parser.addOption(telSchemeOption);
    parser.process(app);

    QString telNumber = parser.value(telSchemeOption);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("DialerAboutData"), QVariant::fromValue(aboutData));

    KDBusService service(KDBusService::Unique, &app);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().at(0));
    Q_ASSERT(window);

    QObject::connect(&service, &KDBusService::activateRequested, window, [=](const QStringList &arguments) {
        for (const auto &arg : arguments) {
            QString numberArg = parseTelArgument(arg);
            if (!numberArg.isEmpty()) {
                inputCallNumber(window, numberArg);
                break;
            }
        }
        KWindowSystem::updateStartupId(window);
        KWindowSystem::activateWindow(window);
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
