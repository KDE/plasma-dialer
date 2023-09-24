// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "ussd-manager.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>

#include "modem-controller.h"
#include "ussd-utils.h"

UssdManager::UssdManager(ModemController *modemController, UssdUtils *ussdUtils, QObject *parent)
    : QObject(parent)
{
    _ussdUtils = ussdUtils;
    _modemController = modemController;

    connect(_modemController, &ModemController::ussdErrorReceived, _ussdUtils, &UssdUtils::errorReceived);
    connect(_modemController, &ModemController::ussdNotificationReceived, _ussdUtils, &UssdUtils::notificationReceived);
    connect(_modemController, &ModemController::ussdRequestReceived, _ussdUtils, &UssdUtils::requestReceived);
    connect(_modemController, &ModemController::ussdInitiateComplete, this, &UssdManager::onInitiateComplete);
    connect(_modemController, &ModemController::ussdStateChanged, this, &UssdManager::onStateChanged);

    connect(_ussdUtils, &UssdUtils::initiated, this, &UssdManager::onInitiated);
    connect(_ussdUtils, &UssdUtils::responded, this, &UssdManager::onResponded);
    connect(_ussdUtils, &UssdUtils::cancelled, this, &UssdManager::onCanceled);
}

bool UssdManager::active() const
{
    return _state != QStringLiteral("idle");
}

QString UssdManager::state() const
{
    return _state;
}

void UssdManager::onInitiated(const QString &deviceUni, const QString &command)
{
    qDebug() << Q_FUNC_INFO << deviceUni << command;
    _modemController->ussdInitiate(deviceUni, command);
}

void UssdManager::onInitiateComplete(const QString &deviceUni, const QString &command)
{
    if (state() == QStringLiteral("user-response")) {
        Q_EMIT _ussdUtils->requestReceived(deviceUni, command);
        return;
    }
    Q_EMIT _ussdUtils->notificationReceived(deviceUni, command);
}

void UssdManager::onResponded(const QString &deviceUni, const QString &reply)
{
    qDebug() << Q_FUNC_INFO << reply;
    _modemController->ussdRespond(deviceUni, reply);
}

void UssdManager::onCanceled(const QString &deviceUni)
{
    qDebug() << Q_FUNC_INFO;
    _modemController->ussdCancel(deviceUni);
}

void UssdManager::onStateChanged(const QString &deviceUni, const QString &state)
{
    _state = state;
    Q_EMIT stateChanged(deviceUni, _state);
    Q_EMIT activeChanged(deviceUni);
}
