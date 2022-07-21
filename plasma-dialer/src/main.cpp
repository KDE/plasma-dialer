// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QGuiApplication>

#include "config.h"
#include "version.h"

#include <KAboutData>
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

#ifdef DIALER_BUILD_SHELL_OVERLAY
#include "qwayland-kde-lockscreen-overlay-v1.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QWaylandClientExtensionTemplate>
#include <qpa/qplatformnativeinterface.h>
#endif // DIALER_BUILD_SHELL_OVERLAY

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

struct ScreenSaverUtils {
    Q_GADGET
public:
    Q_INVOKABLE static bool getActive()
    {
        bool active = false;
#ifdef DIALER_BUILD_SHELL_OVERLAY
        QDBusMessage request = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                              QStringLiteral("/ScreenSaver"),
                                                              QStringLiteral("org.freedesktop.ScreenSaver"),
                                                              QStringLiteral("GetActive"));
        const QDBusReply<bool> response = QDBusConnection::sessionBus().call(request);
        active = response.isValid() ? response.value() : false;
#endif // DIALER_BUILD_SHELL_OVERLAY
        return active;
    }
};
Q_DECLARE_METATYPE(ScreenSaverUtils)

#ifdef DIALER_BUILD_SHELL_OVERLAY
class WaylandAboveLockscreen : public QWaylandClientExtensionTemplate<WaylandAboveLockscreen>, public QtWayland::kde_lockscreen_overlay_v1
{
public:
    WaylandAboveLockscreen()
        : QWaylandClientExtensionTemplate<WaylandAboveLockscreen>(1)
    {
        QMetaObject::invokeMethod(this, "addRegistryListener");
    }

    void allowWindow(QWindow *window)
    {
        QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
        wl_surface *surface = reinterpret_cast<wl_surface *>(native->nativeResourceForWindow(QByteArrayLiteral("surface"), window));
        allow(surface);
    }
};
#endif // DIALER_BUILD_SHELL_OVERLAY

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

    auto config = Config::self();

    qmlRegisterSingletonInstance("org.kde.phone.dialer", 1, 0, "Config", config);

    engine.rootContext()->setContextProperty(QStringLiteral("DialerAboutData"), QVariant::fromValue(aboutData));

    KDBusService service(KDBusService::Unique, &app);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    ScreenSaverUtils screenSaverUtils;
    engine.rootContext()->setContextProperty(QStringLiteral("ScreenSaverUtils"), QVariant::fromValue<ScreenSaverUtils>(screenSaverUtils));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().at(0));
    Q_ASSERT(window);

#ifdef DIALER_BUILD_SHELL_OVERLAY
    WaylandAboveLockscreen aboveLockscreen;
    Q_ASSERT(aboveLockscreen.isInitialized());

    bool screenLocked = ScreenSaverUtils::getActive();
    if (screenLocked) {
        aboveLockscreen.allowWindow(window);
    }
    QObject::connect(&service, &KDBusService::activateRequested, window, [&window, &aboveLockscreen](const QStringList &arguments) {
#else // DIALER_BUILD_SHELL_OVERLAY
    QObject::connect(&service, &KDBusService::activateRequested, window, [&window](const QStringList &arguments) {
#endif // DIALER_BUILD_SHELL_OVERLAY
        for (const auto &arg : arguments) {
            QString numberArg = parseTelArgument(arg);
            if (!numberArg.isEmpty()) {
                inputCallNumber(window, numberArg);
                break;
            }
        }
#ifdef DIALER_BUILD_SHELL_OVERLAY
        bool screenLocked = ScreenSaverUtils::getActive();
        if (screenLocked) {
            aboveLockscreen.allowWindow(window);
            window->setVisibility(QWindow::Visibility::FullScreen);
        }
#endif // DIALER_BUILD_SHELL_OVERLAY
        KWindowSystem::raiseWindow(window->winId());
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
