// SPDX-FileCopyrightText: 2020-2021 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "notification-manager.h"

#include <KLocalizedString>

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
{
}

void NotificationManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    qDebug() << Q_FUNC_INFO;
    _callUtils = callUtils;

    connect(_callUtils, &org::kde::telephony::CallUtils::callAdded, this, &NotificationManager::onCallAdded);
    connect(_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &NotificationManager::onCallStateChanged);
    connect(_callUtils, &org::kde::telephony::CallUtils::callDeleted, this, &NotificationManager::onCallDeleted);
}

void NotificationManager::setContactUtils(ContactUtils *contactUtils)
{
    _contactUtils = contactUtils;
}

void NotificationManager::onCallAdded(const QString &deviceUni,
                                      const QString &callUni,
                                      const DialerTypes::CallDirection &callDirection,
                                      const DialerTypes::CallState &callState,
                                      const DialerTypes::CallStateReason &callStateReason,
                                      const QString communicationWith)
{
    qDebug() << Q_FUNC_INFO << "call added" << deviceUni << callUni << callDirection << callState << callStateReason << communicationWith;
    if (callDirection == DialerTypes::CallDirection::Incoming) {
        if (callState == DialerTypes::CallState::RingingIn) {
            openRingingNotification(deviceUni, callUni, communicationWith);
        }
    }
}

void NotificationManager::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    qDebug() << Q_FUNC_INFO << "call deleted" << deviceUni << callUni;
    closeRingingNotification();
}

void NotificationManager::onCallStateChanged(const QString &deviceUni,
                                             const QString &callUni,
                                             const DialerTypes::CallDirection &callDirection,
                                             const DialerTypes::CallState &callState,
                                             const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << Q_FUNC_INFO << "call state changed:" << deviceUni << callUni << callDirection << callState << callStateReason;
}

void NotificationManager::openRingingNotification(const QString &deviceUni, const QString &callUni, const QString communicationWith)
{
    QString callerDisplayName = _contactUtils->displayString(communicationWith);

    QStringList actions;
    actions << i18n("Accept") << i18n("Reject");
    qDebug() << Q_FUNC_INFO << _ringingNotification;
    if (!_ringingNotification) {
        qDebug() << Q_FUNC_INFO << "new notification";
        _ringingNotification = new KNotification(QStringLiteral("ringing"), KNotification::Persistent | KNotification::LoopSound, nullptr);
        void (KNotification::*activation)(unsigned int) = &KNotification::activated;
        connect(_ringingNotification, activation, this, &NotificationManager::onNotificationAction);
    }
    _ringingNotification->setUrgency(KNotification::CriticalUrgency);
    _ringingNotification->setComponentName(QStringLiteral("plasma_dialer"));
    // _ringingNotification->setPixmap(person.photo());
    _ringingNotification->setTitle(i18n("Incoming call"));
    _ringingNotification->setText(callerDisplayName);
    // this will be used by the notification applet to show custom notification UI
    // with swipe decision.
    _ringingNotification->setHint(QStringLiteral("category"), QStringLiteral("x-kde.incoming-call"));
    _ringingNotification->setActions(actions);
    _ringingNotification->addContext(QStringLiteral("deviceUni"), deviceUni);
    _ringingNotification->addContext(QStringLiteral("callUni"), callUni);
    _ringingNotification->sendEvent();
}

void NotificationManager::closeRingingNotification()
{
    if (!_ringingNotification) {
        return;
    }
    _ringingNotification->disconnect();
    _ringingNotification->close();
    _ringingNotification = nullptr;
}

void NotificationManager::accept(const QString &deviceUni, const QString &callUni)
{
    QDBusPendingReply<> reply = _callUtils->accept(deviceUni, callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    closeRingingNotification();
}

void NotificationManager::hangUp(const QString &deviceUni, const QString &callUni)
{
    QDBusPendingReply<> reply = _callUtils->hangUp(deviceUni, callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    closeRingingNotification();
}

void NotificationManager::onNotificationAction(unsigned int action)
{
    qDebug() << Q_FUNC_INFO << action;
    QString deviceUni;
    QString callUni;
    KNotification::ContextList notificationContexts = _ringingNotification->contexts();
    for (const auto &context : notificationContexts) {
        if (context.first == QStringLiteral("deviceUni")) {
            deviceUni = context.second;
        }
        if (context.first == QStringLiteral("callUni")) {
            callUni = context.second;
        }
    }
    switch (action) {
    case 1:
        accept(deviceUni, callUni);
        QProcess::startDetached(QStringLiteral("plasmaphonedialer"), QStringList{});
        break;
    case 2:
        hangUp(deviceUni, callUni);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}
