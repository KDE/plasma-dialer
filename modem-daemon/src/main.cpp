// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include <QCoreApplication>

#include "version.h"

#include "callhistorydatabaseadaptor.h"
#include "callutilsadaptor.h"
#include "deviceutilsadaptor.h"
#include "ussdutilsadaptor.h"

#include "call-history-manager.h"
#include "call-manager.h"
#include "device-manager.h"
#include "ussd-manager.h"

#include "mm-modem-controller.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    app.setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setApplicationName(QStringLiteral("modem-daemon"));

    ModemManagerController modemController;
    DialerTypes::registerMetaTypes();

    auto deviceUtils = new DeviceUtils(&app);
    auto ussdUtils = new UssdUtils(&app);
    auto callHistoryDatabase = new CallHistoryDatabase(&app);
    auto callUtils = new CallUtils(&app);

    DeviceManager deviceManager(&modemController, deviceUtils, &app);
    UssdManager ussdManager(&modemController, ussdUtils, &app);
    CallHistoryManager callHistoryManager(&modemController, callHistoryDatabase, &app);
    CallManager callManager(&modemController, callUtils, &app);

    QDBusConnection dbus = QDBusConnection::sessionBus();

    new DeviceUtilsAdaptor(deviceUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/DeviceUtils/tel/mm"), deviceUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.DeviceUtils"));

    new UssdUtilsAdaptor(ussdUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/UssdUtils/tel/mm"), ussdUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.UssdUtils"));

    new CallHistoryDatabaseAdaptor(callHistoryDatabase);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/CallHistoryDatabase/tel/mm"), callHistoryDatabase);
    dbus.registerService(QStringLiteral("org.kde.telephony.CallHistoryDatabase"));

    new CallUtilsAdaptor(callUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/CallUtils/tel/mm"), callUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.CallUtils"));

    return app.exec();
}
