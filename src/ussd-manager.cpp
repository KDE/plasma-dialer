// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "ussd-manager.h"
#include "dialerutils.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>

#include <KLocalizedString>

#define CANONICAL_TELEPHONY_USSD_IFACE "com.canonical.Telephony.USSD"

struct UssdManager::Private {
    Tp::ConnectionPtr connection;
    DialerUtils *dialerUtils;
    QDBusInterface *ussdInterface;
    QString state;
};

UssdManager::UssdManager(const Tp::ConnectionPtr &connection, DialerUtils *dialerUtils, QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    if (connection.isNull()) {
        qCritical() << Q_FUNC_INFO;
        return;
    }

    d->dialerUtils = dialerUtils;
    d->connection = connection;

    d->ussdInterface =
        new QDBusInterface(d->connection->busName(), d->connection->objectPath(), CANONICAL_TELEPHONY_USSD_IFACE, QDBusConnection::sessionBus(), this);

    if (!d->ussdInterface->isValid()) {
        qDebug() << Q_FUNC_INFO;
        return;
    }

    connect(d->dialerUtils, &DialerUtils::initiateUssd, this, &UssdManager::onInitiated);
    connect(d->dialerUtils, &DialerUtils::respondToUssd, this, &UssdManager::onResponded);
    connect(d->dialerUtils, &DialerUtils::cancelUssd, this, &UssdManager::onCanceled);

    QDBusConnection::sessionBus().connect(d->connection->busName(),
                                          d->connection->objectPath(),
                                          CANONICAL_TELEPHONY_USSD_IFACE,
                                          QStringLiteral("NotificationReceived"),
                                          d->dialerUtils,
                                          SIGNAL(notificationReceivedFromUssd(QString)));

    QDBusConnection::sessionBus().connect(d->connection->busName(),
                                          d->connection->objectPath(),
                                          CANONICAL_TELEPHONY_USSD_IFACE,
                                          QStringLiteral("RequestReceived"),
                                          d->dialerUtils,
                                          SIGNAL(requestReceivedFromUssd(QString)));

    QDBusConnection::sessionBus().connect(d->connection->busName(),
                                          d->connection->objectPath(),
                                          CANONICAL_TELEPHONY_USSD_IFACE,
                                          QStringLiteral("InitiateUSSDComplete"),
                                          this,
                                          SLOT(onInitiateComplete(QString)));

    QDBusConnection::sessionBus().connect(d->connection->busName(),
                                          d->connection->objectPath(),
                                          CANONICAL_TELEPHONY_USSD_IFACE,
                                          QStringLiteral("StateChanged"),
                                          this,
                                          SLOT(onStateChanged(QString)));
}

UssdManager::~UssdManager()
{
    qDebug() << "Deleting UssdManager";
}

bool UssdManager::active() const
{
    return d->state != QStringLiteral("idle");
}

QString UssdManager::state() const
{
    return d->state;
}

void UssdManager::onInitiated(const QString &command)
{
    qDebug() << Q_FUNC_INFO << command;
    d->ussdInterface->asyncCall(QStringLiteral("Initiate"), command);
}

void UssdManager::onInitiateComplete(const QString &command)
{
    qDebug() << Q_FUNC_INFO;
    if (state() == QStringLiteral("user-response")) {
        Q_EMIT d->dialerUtils->requestReceivedFromUssd(command);
        return;
    }
    Q_EMIT d->dialerUtils->notificationReceivedFromUssd(command);
}

void UssdManager::onResponded(const QString &reply)
{
    qDebug() << Q_FUNC_INFO << reply;
    d->ussdInterface->asyncCall(QStringLiteral("Respond"), reply);
}

void UssdManager::onCanceled()
{
    qDebug() << Q_FUNC_INFO;
    d->ussdInterface->asyncCall(QStringLiteral("Cancel"));
}

void UssdManager::onStateChanged(const QString &state)
{
    d->state = state;
    Q_EMIT stateChanged(d->state);
    Q_EMIT activeChanged();
}
