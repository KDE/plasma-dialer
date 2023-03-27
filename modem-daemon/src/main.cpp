// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include <QCoreApplication>

#include "version.h"

#include "callutilsadaptor.h"
#include "deviceutilsadaptor.h"
#include "ussdutilsadaptor.h"

#include "call-manager.h"
#include "device-manager.h"
#include "ussd-manager.h"

#if defined(MODEM_SUBSYSTEM_MM)
#include "mm-modem-controller.h"
#elif defined(MODEM_SUBSYSTEM_OFONO)
// TODO: oFono modem controller
#else
#include "modem-controller.h"
#endif

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    app.setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setApplicationName(QStringLiteral("modem-daemon"));

#if defined(MODEM_SUBSYSTEM_MM)
    ModemManagerController modemController;
#elif defined(MODEM_SUBSYSTEM_OFONO)
    // TODO: oFono modem controller
#else
    ModemController modemController;
#endif
    DialerTypes::registerMetaTypes();

    auto deviceUtils = new DeviceUtils(&app);
    auto ussdUtils = new UssdUtils(&app);
    auto callUtils = new CallUtils(&app);

    DeviceManager deviceManager(&modemController, deviceUtils, &app);
    UssdManager ussdManager(&modemController, ussdUtils, &app);
    CallManager callManager(&modemController, callUtils, &app);

    QDBusConnection dbus = QDBusConnection::sessionBus();
    auto protocol = modemController.protocol();
    auto subsystem = modemController.subsystem();

    new DeviceUtilsAdaptor(deviceUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/DeviceUtils/") + protocol + QStringLiteral("/") + subsystem, deviceUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.DeviceUtils"));

    new UssdUtilsAdaptor(ussdUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/UssdUtils/") + protocol + QStringLiteral("/") + subsystem, ussdUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.UssdUtils"));

    new CallUtilsAdaptor(callUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/CallUtils/") + protocol + QStringLiteral("/") + subsystem, callUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.CallUtils"));

    qDebug() << Q_FUNC_INFO << protocol << subsystem;

    return app.exec();
}
