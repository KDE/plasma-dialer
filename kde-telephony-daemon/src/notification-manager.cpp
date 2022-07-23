// SPDX-FileCopyrightText: 2020-2021 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "notification-manager.h"

#include <KLocalizedString>
#include <MprisQt/MprisController>
#include <QTimer>

static void waitForControllerInit(MprisController *mprisController)
{
    qDebug() << Q_FUNC_INFO << mprisController->service() << mprisController->isValid();
    QTimer timer; // as a precaution
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(mprisController, &MprisController::playbackStatusChanged, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(300);
    loop.exec();
}

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
    , _ringEffect(std::make_unique<QFeedbackHapticsEffect>())
    , _mprisManager(this)
{
    _ringEffect->setAttackIntensity(0.1);
    _ringEffect->setAttackTime(420);
    _ringEffect->setIntensity(0.7);
    _ringEffect->setDuration(5000);
    _ringEffect->setFadeTime(700);
    _ringEffect->setFadeIntensity(0.07);
    _ringEffect->setPeriod(1300);
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
            handleIncomingCall(deviceUni, callUni, communicationWith);
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
    if (callDirection == DialerTypes::CallDirection::Incoming) {
        if (callState == DialerTypes::CallState::Terminated) {
            handleRejectedCall();
        }
    }
}

void NotificationManager::openRingingNotification(const QString &deviceUni, const QString &callUni, const QString communicationWith)
{
    _ringEffect->start();
    QProcess::startDetached(QStringLiteral("plasmaphonedialer"), QStringList{});
    //for our research goals for now we don't need nothing of below code
    /*
    QString contactName = _contactUtils->displayString(communicationWith);
    QString callerDisplayString =
        (contactName == communicationWith) ? communicationWith : communicationWith + QStringLiteral("<br>") + QStringLiteral("<b>%1</b>").arg(contactName);

    QStringList actions;
    actions << i18n("Accept") << i18n("Reject");
    qDebug() << Q_FUNC_INFO << _ringingNotification;
    if (!_ringingNotification) {
        qDebug() << Q_FUNC_INFO << "new notification";
        _ringingNotification = new KNotification(QStringLiteral("ringing"), KNotification::Persistent | KNotification::LoopSound, nullptr);
    }
    _ringingNotification->setUrgency(KNotification::CriticalUrgency);
    _ringingNotification->setComponentName(QStringLiteral("plasma_dialer"));
    // _ringingNotification->setPixmap(person.photo());
    _ringingNotification->setTitle(i18n("Incoming call"));
    _ringingNotification->setText(callerDisplayString);
    // this will be used by the notification applet to show custom notification UI
    // with swipe decision.
    _ringingNotification->setHint(QStringLiteral("category"), QStringLiteral("x-kde.incoming-call"));
    _ringingNotification->setActions(actions);
    _ringingNotification->addContext(QStringLiteral("deviceUni"), deviceUni);
    _ringingNotification->addContext(QStringLiteral("callUni"), callUni);
    connect(_ringingNotification, QOverload<unsigned int>::of(&KNotification::activated), this, &NotificationManager::onNotificationAction);
    _ringingNotification->sendEvent();
    */
}

void NotificationManager::closeRingingNotification()
{
    if (!_ringingNotification) {
        return;
    }
    _ringingNotification->disconnect();
    _ringingNotification->close();
    _ringingNotification = nullptr;
    _ringEffect->stop();
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

void NotificationManager::handleIncomingCall(const QString &deviceUni, const QString &callUni, const QString communicationWith)
{
    pauseMedia();
    openRingingNotification(deviceUni, callUni, communicationWith);
}

void NotificationManager::handleRejectedCall()
{
    closeRingingNotification();
    unpauseMedia();
}

void NotificationManager::pauseMedia()
{
    if (_mprisManager.playbackStatus() != Mpris::Playing) {
        return;
    }

    auto services = _mprisManager.availableServices();
    for (auto service : services) {
        qDebug() << service;
        if (service.contains(QLatin1String("kdeconnect"))) {
            qDebug() << Q_FUNC_INFO << "Skip players connected over KDE Connect. KDE Connect pauses them itself";
            continue;
        }
        MprisController mprisController(service, QDBusConnection::sessionBus(), this);
        waitForControllerInit(&mprisController);
        if (mprisController.playbackStatus() == Mpris::Playing) {
            if (!_pausedSources.contains(service)) {
                _pausedSources.insert(service);
                if (mprisController.canPause()) {
                    mprisController.pause();
                } else {
                    mprisController.stop();
                }
            }
        }
    }
}

void NotificationManager::unpauseMedia()
{
    for (const QString &service : qAsConst(_pausedSources)) {
        MprisController mprisController(service, QDBusConnection::sessionBus(), this);
        waitForControllerInit(&mprisController);
        mprisController.play();
    }
    _pausedSources.clear();
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
