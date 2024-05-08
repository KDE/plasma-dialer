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
    m_ussdUtils = ussdUtils;
    m_modemController = modemController;

    connect(m_modemController, &ModemController::ussdErrorReceived, m_ussdUtils, &UssdUtils::errorReceived);
    connect(m_modemController, &ModemController::ussdNotificationReceived, m_ussdUtils, &UssdUtils::notificationReceived);
    connect(m_modemController, &ModemController::ussdRequestReceived, m_ussdUtils, &UssdUtils::notificationReceived);
    connect(m_modemController, &ModemController::ussdInitiateComplete, m_ussdUtils, &UssdUtils::notificationReceived);
    connect(m_modemController, &ModemController::ussdStateChanged, this, &UssdManager::onStateChanged);

    connect(m_ussdUtils, &UssdUtils::initiated, this, &UssdManager::onInitiated);
    connect(m_ussdUtils, &UssdUtils::responded, this, &UssdManager::onResponded);
    connect(m_ussdUtils, &UssdUtils::cancelled, this, &UssdManager::onCanceled);
}

bool UssdManager::active() const
{
    return m_state != QStringLiteral("idle");
}

QString UssdManager::state() const
{
    return m_state;
}

void UssdManager::onInitiated(const QString &deviceUni, const QString &command)
{
    qDebug() << Q_FUNC_INFO << deviceUni << command;
    m_modemController->ussdInitiate(deviceUni, command);
}

void UssdManager::onResponded(const QString &deviceUni, const QString &reply)
{
    qDebug() << Q_FUNC_INFO << reply;
    m_modemController->ussdRespond(deviceUni, reply);
}

void UssdManager::onCanceled(const QString &deviceUni)
{
    qDebug() << Q_FUNC_INFO;
    m_modemController->ussdCancel(deviceUni);
}

void UssdManager::onStateChanged(const QString &deviceUni, const QString &state)
{
    m_state = state;
    Q_EMIT stateChanged(deviceUni, m_state);
    Q_EMIT activeChanged(deviceUni);
    Q_EMIT m_ussdUtils->stateChanged(deviceUni, m_state);
}
