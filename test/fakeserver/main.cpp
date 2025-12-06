// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QString>

#include <KAboutData>

#include "server.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));

    // start wizard
    KAboutData aboutData(QStringLiteral("plasma-dialer-fakeserver"),
                         QStringLiteral("Plasma Dialer Fakeserver"),
                         QStringLiteral("1.0"),
                         QStringLiteral(""),
                         KAboutLicense::GPL,
                         QStringLiteral("© 2025 KDE Community"));
    aboutData.addAuthor(QStringLiteral("Devin Lin"), QString(), QStringLiteral("devin@kde.org"));
    KAboutData::setApplicationData(aboutData);

    QQmlApplicationEngine engine;

    Server::instance();

    qmlRegisterSingletonType<Server>("plasmadialerfakeserver", 1, 0, "Server", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return Server::instance();
    });

    engine.loadFromModule(QStringLiteral("org.kde.plasma.dialer.fakeserver"), QStringLiteral("Main"));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("start-here-symbolic")));

    return app.exec();
}
