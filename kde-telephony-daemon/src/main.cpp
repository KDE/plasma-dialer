// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include <KLocalizedString>
#include <QCoreApplication>
#include <QtDBus/QDBusConnection>

#include "version.h"

#include "contact-utils.h"
#include "dialer-utils.h"

#include "callutilsinterface.h"
#include "deviceutilsinterface.h"

#include "contactutilsadaptor.h"
#include "dialerutilsadaptor.h"

#include "dialer-manager.h"
#include "notification-manager.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    app.setApplicationVersion(QStringLiteral(PLASMADIALER_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("kde.org"));

    KLocalizedString::setApplicationDomain("kde-telephony-daemon");

    auto deviceUtils = new org::kde::telephony::DeviceUtils(QString::fromLatin1(org::kde::telephony::DeviceUtils::staticInterfaceName()),
                                                            QStringLiteral("/org/kde/telephony/DeviceUtils/tel/mm"),
                                                            QDBusConnection::sessionBus(),
                                                            &app);

    auto callUtils = new org::kde::telephony::CallUtils(QString::fromLatin1(org::kde::telephony::CallUtils::staticInterfaceName()),
                                                        QStringLiteral("/org/kde/telephony/CallUtils/tel/mm"),
                                                        QDBusConnection::sessionBus(),
                                                        &app);

    auto contactUtils = new ContactUtils(&app);
    auto dialerUtils = new DialerUtils(&app);

    DialerTypes::registerMetaTypes();

    QObject::connect(deviceUtils, &org::kde::telephony::DeviceUtils::countryCodeChanged, contactUtils, &ContactUtils::changeCountryCode);

    QDBusConnection dbus = QDBusConnection::sessionBus();

    DialerManager dialerManager(&app);
    dialerManager.setDialerUtils(dialerUtils);
    dialerManager.setCallUtils(callUtils);

    NotificationManager notificator(&app);
    notificator.setCallUtils(callUtils);
    notificator.setContactUtils(contactUtils);

    new ContactUtilsAdaptor(contactUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/ContactUtils/tel/mm"), contactUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.ContactUtils"));

    new DialerUtilsAdaptor(dialerUtils);
    dbus.registerObject(QStringLiteral("/org/kde/telephony/DialerUtils/tel/mm"), dialerUtils);
    dbus.registerService(QStringLiteral("org.kde.telephony.DialerUtils"));

    return app.exec();
}
