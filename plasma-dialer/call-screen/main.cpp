// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QGuiApplication>

#include "config.h"
#include "lockscreenutils.h"
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

#ifdef DIALER_BUILD_SHELL_OVERLAY
#include "qwayland-kde-lockscreen-overlay-v1.h"
#include <KWaylandExtras>
#include <KWindowSystem>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QWaylandClientExtensionTemplate>
#include <qpa/qplatformnativeinterface.h>

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
        if (!surface) {
            qWarning() << "Failed to retrieve surface to allow lockscreen overlay, returned nullptr";
            return;
        }
        allow(surface);
    }
};
#endif // DIALER_BUILD_SHELL_OVERLAY

static void allowAboveLockscreen(QWindow *window)
{
    #ifdef DIALER_BUILD_SHELL_OVERLAY
    if (KWindowSystem::isPlatformWayland()) {
        WaylandAboveLockscreen aboveLockscreen;
        Q_ASSERT(aboveLockscreen.isInitialized());
        if (!aboveLockscreen.isInitialized()) {
            qDebug() << "failed to initialize lockscreen overlay object";
            return;
        }
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
    bool screenLocked = LockScreenUtils::instance()->lockscreenActive();
    if (screenLocked) {
        if (KWindowSystem::isPlatformWayland()) {
            window->setVisibility(QWindow::Visibility::FullScreen);
            KWaylandExtras::requestXdgActivationToken(window, 0, QStringLiteral("org.kde.plasma.callui.desktop"));
            QObject::connect(KWaylandExtras::self(), &KWaylandExtras::xdgActivationTokenArrived, window, [window](int, const QString &token) {
                KWindowSystem::setCurrentXdgActivationToken(token);
                KWindowSystem::activateWindow(window);
            });
        } else {
            qWarning() << Q_FUNC_INFO << "Screen is locked. Dialer shell overlay is supported only for Wayland";
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
    KLocalizedString::setApplicationDomain("plasma-call-screen");
    QCoreApplication::setApplicationName(QStringLiteral("callscreen"));
    QCoreApplication::setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("callscreen")));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("Phone"));

    KAboutData aboutData(QStringLiteral("callscreen"),
                         i18n("Phone"),
                         QStringLiteral(PLASMADIALER_VERSION_STRING),
                         i18n("Plasma call screen"),
                         KAboutLicense::GPL,
                         i18n("© 2015-2022 KDE Community"));
    aboutData.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=Plasma%20Mobile%20Dialer");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.plasma.callscreen"));
    aboutData.addAuthor(i18n("Alexey Andreyev"), QString(), QStringLiteral("aa13q@ya.ru"));
    KAboutData::setApplicationData(aboutData);

    KCrash::initialize();

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);

    auto config = Config::self();
    engine.rootContext()->setContextProperty(QStringLiteral("DialerAboutData"), QVariant::fromValue(aboutData));
    engine.rootContext()->setContextProperty(QStringLiteral("Config"), QVariant::fromValue(config));

    KDBusService service(KDBusService::Unique, &app);

    engine.loadFromModule(QStringLiteral("org.kde.plasma.callscreen"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().at(0));
    Q_ASSERT(window);

    // Register with lockscreen overlay protocol at startup before window is visible
    allowAboveLockscreen(window);

    QObject::connect(&service, &KDBusService::activateRequested, window, [&window](const QStringList &arguments) {
        qDebug() << "Window activation requested over DBus";

        raiseWindow(window);
        window->requestActivate();
    });

    return app.exec();
}


