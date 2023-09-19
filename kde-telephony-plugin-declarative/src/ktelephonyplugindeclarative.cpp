// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "ktelephonyplugindeclarative.h"

#include <QAbstractItemModel>
#include <QPluginLoader>
#include <QQmlEngine>

#include "active-call-model.h"
#include "call-history-model.h"
#include "declarative-call-utils.h"
#include "declarative-contact-utils.h"
#include "declarative-device-utils.h"
#include "declarative-dialer-utils.h"
#include "declarative-ussd-utils.h"

static org::kde::telephony::DeviceUtils *deviceUtils = nullptr;

static DeclarativeCallUtils *declarativeCallUtils = nullptr;

static QObject *ussdUtilsTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto ussdUtils = new DeclarativeUssdUtils();
    return ussdUtils;
}

static QObject *deviceUtilsTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    if (deviceUtils == nullptr) {
        deviceUtils = new org::kde::telephony::DeviceUtils(QString::fromLatin1(org::kde::telephony::DeviceUtils::staticInterfaceName()),
                                                           QStringLiteral("/org/kde/telephony/DeviceUtils/tel/mm"),
                                                           QDBusConnection::sessionBus(),
                                                           engine);
    }
    auto declDeviceUtils = new DeclarativeDeviceUtils();
    declDeviceUtils->setDeviceUtils(deviceUtils);
    return declDeviceUtils;
}

static QObject *callUtilsTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    if (declarativeCallUtils == nullptr) {
        declarativeCallUtils = new DeclarativeCallUtils();
    }

    return declarativeCallUtils;
}

static QAbstractListModel *callHistoryModelTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto callHistoryModel = new CallHistoryModel();
    return callHistoryModel;
}

static QAbstractListModel *activeCallModelTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto activeCallModel = new ActiveCallModel();

    if (declarativeCallUtils == nullptr) {
        declarativeCallUtils = new DeclarativeCallUtils();
    }
    auto callUtils = qobject_cast<org::kde::telephony::CallUtils *>(declarativeCallUtils);

    activeCallModel->setCallUtils(callUtils);
    return activeCallModel;
}

static QObject *dialerUtilsTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto dialerUtils = new DeclarativeDialerUtils();
    return dialerUtils;
}

static QObject *contactUtilsTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto contactUtils = new DeclarativeContactUtils();
    return contactUtils;
}

void KTelephonyPluginDeclarative::registerTypes(const char *uri)
{
    if (QString::fromLocal8Bit(uri) != QLatin1String("org.kde.telephony")) {
        return;
    }

    int major = 1;
    int minor = 0;

    DialerTypes::registerMetaTypes();
    qmlRegisterAnonymousType<QAbstractItemModel>(uri, 1);
    qmlRegisterUncreatableMetaObject(DialerTypes::staticMetaObject, uri, major, minor, "DialerTypes", QStringLiteral("is enum"));

    // calls-daemon

    qmlRegisterSingletonType<DeclarativeUssdUtils>(uri, major, minor, "UssdUtils", ussdUtilsTypeProvider);
    qmlRegisterSingletonType<DeclarativeDeviceUtils>(uri, major, minor, "DeviceUtils", deviceUtilsTypeProvider);
    qmlRegisterSingletonType<DeclarativeCallUtils>(uri, major, minor, "CallUtils", callUtilsTypeProvider);

    qmlRegisterSingletonType<CallHistoryModel>(uri, major, minor, "CallHistoryModel", callHistoryModelTypeProvider);
    qmlRegisterSingletonType<ActiveCallModel>(uri, major, minor, "ActiveCallModel", activeCallModelTypeProvider);

    // plugin-only KContacts-related utils
    qRegisterMetaType<KContacts::PhoneNumber>();

    // kde-telephony-daemon

    qmlRegisterSingletonType<DeclarativeDialerUtils>(uri, major, minor, "DialerUtils", dialerUtilsTypeProvider);
    qmlRegisterSingletonType<DeclarativeContactUtils>(uri, major, minor, "ContactUtils", contactUtilsTypeProvider);
}
