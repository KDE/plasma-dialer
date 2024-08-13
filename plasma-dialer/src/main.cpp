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
#include <QCommandLineParser>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#ifdef DIALER_BUILD_SHELL_OVERLAY
#include "qwayland-kde-lockscreen-overlay-v1.h"
#include <KWaylandExtras>
#include <KWindowSystem>
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

#ifdef DIALER_BUILD_SHELL_OVERLAY
static void updateLockscreenMode(QWindow *window, bool mode)
{
    QMetaObject::invokeMethod(window, "updateLockscreenMode", Q_ARG(QVariant, mode));
}
#endif // DIALER_BUILD_SHELL_OVERLAY

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
        initialize();
    }

    void allowWindow(QWindow *window)
    {
        QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
        wl_surface *surface = reinterpret_cast<wl_surface *>(native->nativeResourceForWindow(QByteArrayLiteral("surface"), window));

        Q_ASSERT(surface);
        allow(surface);
    }
};
#endif // DIALER_BUILD_SHELL_OVERLAY

static void allowAboveLockscreen(QWindow *window)
{
#ifdef DIALER_BUILD_SHELL_OVERLAY
    if (KWindowSystem::isPlatformWayland()) {
        Q_ASSERT(!window->isVisible());
        WaylandAboveLockscreen aboveLockscreen;
        Q_ASSERT(aboveLockscreen.isInitialized());
        aboveLockscreen.allowWindow(window);
    } else {
        qDebug() << Q_FUNC_INFO << "Dialer shell overlay is supported only for Wayland";
    }
#endif // DIALER_BUILD_SHELL_OVERLAY
}

// raiseWindow with lockscreen support if possible
static void raiseWindow(QWindow *window)
{
#ifdef DIALER_BUILD_SHELL_OVERLAY
    bool screenLocked = ScreenSaverUtils::getActive();
    updateLockscreenMode(window, screenLocked);
    if (screenLocked) {
        if (KWindowSystem::isPlatformWayland()) {
            window->setVisibility(QWindow::Visibility::FullScreen);
            KWaylandExtras::requestXdgActivationToken(window, 0, QStringLiteral("org.kde.plasma.dialer.desktop"));
            QObject::connect(KWaylandExtras::self(), &KWaylandExtras::xdgActivationTokenArrived, window, [window](int, const QString &token) {
                KWindowSystem::setCurrentXdgActivationToken(token);
                KWindowSystem::activateWindow(window);
            });
        } else {
            qDebug() << Q_FUNC_INFO << "Screen is locked. Dialer shell overlay is supported only for Wayland";
        }
    } else {
        window->raise();
    }
#else // DIALER_BUILD_SHELL_OVERLAY
    window->raise();
#endif // DIALER_BUILD_SHELL_OVERLAY
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

    QCommandLineParser parser;
    QCommandLineOption telSchemeOption(QStringList() << QStringLiteral("number"),
                                       QCoreApplication::translate("main", "Input a phone number"),
                                       QCoreApplication::translate("main", "phone number"));
    parser.addOption(telSchemeOption);
    parser.process(app);

    QString telNumber = parser.value(telSchemeOption);

    QQmlApplicationEngine engine;

    auto config = Config::self();

    qmlRegisterSingletonInstance("org.kde.plasma.dialer", 1, 0, "Config", config);

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

    allowAboveLockscreen(window);

    raiseWindow(window);
    QObject::connect(&service, &KDBusService::activateRequested, window, [&window](const QStringList &arguments) {
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
