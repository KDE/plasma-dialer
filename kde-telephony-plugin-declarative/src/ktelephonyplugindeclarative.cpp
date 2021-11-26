// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "ktelephonyplugindeclarative.h"

#include <QAbstractItemModel>
#include <QPluginLoader>
#include <QQmlEngine>

#include "active-call-model.h"
#include "call-history-model.h"
#include "callutilsinterface.h"
#include "declarative-contact-utils.h"
#include "declarative-device-utils.h"
#include "declarative-dialer-utils.h"
#include "declarative-ussd-utils.h"

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

    auto deviceUtils = new DeclarativeDeviceUtils();
    return deviceUtils;
}

static QObject *callUtilsTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto callUtils = new DeclarativeCallUtils();
    return callUtils;
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

    DialerTypes::registerMetaTypes();
    qmlRegisterAnonymousType<QAbstractItemModel>(uri, 1);
    qmlRegisterUncreatableMetaObject(DialerTypes::staticMetaObject, uri, 1, 0, "DialerTypes", QStringLiteral("is enum"));

    // calls-daemon

    qmlRegisterSingletonType<DeclarativeUssdUtils>(uri, 1, 0, "UssdUtils", ussdUtilsTypeProvider);
    qmlRegisterSingletonType<DeclarativeDeviceUtils>(uri, 1, 0, "DeviceUtils", deviceUtilsTypeProvider);
    qmlRegisterSingletonType<DeclarativeCallUtils>(uri, 1, 0, "CallUtils", callUtilsTypeProvider);

    qmlRegisterSingletonType<CallHistoryModel>(uri, 1, 0, "CallHistoryModel", callHistoryModelTypeProvider);
    qmlRegisterSingletonType<ActiveCallModel>(uri, 1, 0, "ActiveCallModel", activeCallModelTypeProvider);

    // kde-telephony-daemon

    qmlRegisterSingletonType<DeclarativeDialerUtils>(uri, 1, 0, "DialerUtils", dialerUtilsTypeProvider);
    qmlRegisterSingletonType<DeclarativeContactUtils>(uri, 1, 0, "ContactUtils", contactUtilsTypeProvider);
}
