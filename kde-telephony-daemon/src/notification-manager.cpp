// SPDX-FileCopyrightText: 2020-2021 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "notification-manager.h"
#include "config.h"

#include <KIO/ApplicationLauncherJob>
#include <KLocalizedString>
#include <QDBusConnection>
#include <QTimer>

static bool getScreenSaverActive()
{
    bool active = false;
#ifdef DIALER_BUILD_SHELL_OVERLAY
    QDBusMessage request = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("/ScreenSaver"),
                                                          QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("GetActive"));
    const QDBusReply<bool> response = QDBusConnection::sessionBus().call(request);
    active = response.isValid() ? response.value() : false;
#endif // DIALER_BUILD_SHELL_OVERLAY
    return active;
}

static void launchPlasmaDialerDesktopFile()
{
    const auto desktopName = QStringLiteral("org.kde.phone.dialer");
    const KService::Ptr appService = KService::serviceByDesktopName(desktopName);
    if (!appService) {
        qWarning() << "Could not find" << desktopName;
        return;
    }
    KIO::ApplicationLauncherJob job(appService);
    job.start();
}

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
    , _ringingNotification(std::make_unique<KNotification>(QStringLiteral("ringing"), KNotification::Persistent | KNotification::LoopSound, nullptr))
    , _callStarted(false)

#ifdef HAVE_QT5_FEEDBACK
    , _ringEffect(std::make_unique<QFeedbackHapticsEffect>())
#endif // HAVE_QT5_FEEDBACK
{
#ifdef HAVE_QT5_FEEDBACK
    _ringEffect->setAttackIntensity(0.1);
    _ringEffect->setAttackTime(420);
    _ringEffect->setIntensity(0.7);
    _ringEffect->setDuration(5000);
    _ringEffect->setFadeTime(700);
    _ringEffect->setFadeIntensity(0.07);
    _ringEffect->setPeriod(1300);
#endif // HAVE_QT5_FEEDBACK

    _databaseInterface = new org::kde::telephony::CallHistoryDatabase(QString::fromLatin1(_databaseInterface->staticInterfaceName()),
                                                                      QStringLiteral("/org/kde/telephony/CallHistoryDatabase/tel/mm"),
                                                                      QDBusConnection::sessionBus(),
                                                                      this);

    if (!_databaseInterface->isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallHistoryDatabase interface";
        return;
    }
    _ringingNotification->setAutoDelete(false);
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
    qDebug() << Q_FUNC_INFO << "call added" << deviceUni << callUni << callDirection << callState << callStateReason;
    if (callDirection == DialerTypes::CallDirection::Incoming) {
        if (callState == DialerTypes::CallState::RingingIn) {
            handleIncomingCall(deviceUni, callUni, communicationWith);
            _callStarted = false;
        }
    }
}

void NotificationManager::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    qDebug() << Q_FUNC_INFO << "call deleted" << deviceUni << callUni;
    handleCallInteraction();
}

void NotificationManager::onCallStateChanged(const QString &deviceUni,
                                             const QString &callUni,
                                             const DialerTypes::CallDirection &callDirection,
                                             const DialerTypes::CallState &callState,
                                             const DialerTypes::CallStateReason &callStateReason,
                                             const QString &communicationWith)
{
    qDebug() << Q_FUNC_INFO << "call state changed:" << deviceUni << callUni << callDirection << callState << callStateReason;

    const QString contactName = _contactUtils->displayString(communicationWith);
    QString callerDisplay =
        (contactName == communicationWith) ? communicationWith : communicationWith + QStringLiteral("<br>") + QStringLiteral("<b>%1</b>").arg(contactName);
    if (callerDisplay.isEmpty()) {
        callerDisplay = i18n("No Caller ID");
    }

    if (callDirection == DialerTypes::CallDirection::Incoming) {
        if (callState == DialerTypes::CallState::Terminated) {
            handleCallInteraction();

            if (callStateReason == DialerTypes::CallStateReason::Unknown && !_callStarted) {
                auto _missedCallNotification = new KNotification(QStringLiteral("callMissed"));
                _missedCallNotification->setComponentName(QStringLiteral("plasma_dialer"));
                _missedCallNotification->setTitle(i18n("Missed call"));
                _missedCallNotification->setText(i18n("Missed call from %1", callerDisplay));
                _missedCallNotification->sendEvent();
            }
        }
        if (callState == DialerTypes::CallState::Active) {
            handleCallInteraction();
            _callStarted = true;
        }
    }
}

void NotificationManager::openRingingNotification(const QString &deviceUni,
                                                  const QString &callUni,
                                                  const QString callerDisplay,
                                                  const QString notificationEvent)
{
    QStringList actions;
    actions << i18n("Accept") << i18n("Reject");

    _ringingNotification->setEventId(notificationEvent);
    _ringingNotification->setUrgency(KNotification::CriticalUrgency);
    _ringingNotification->setComponentName(QStringLiteral("plasma_dialer"));

    // _ringingNotification->setPixmap(person.photo());
    _ringingNotification->setTitle(i18n("Incoming call"));
    _ringingNotification->setText(callerDisplay);
    // this will be used by the notification applet to show custom notification UI
    // with swipe decision.
    _ringingNotification->setHint(QStringLiteral("category"), QStringLiteral("x-kde.incoming-call"));
    _ringingNotification->setActions(actions);
    connect(_ringingNotification.get(), QOverload<unsigned int>::of(&KNotification::activated), this, &NotificationManager::onNotificationAction);
    _ringingNotification->sendEvent();
}

void NotificationManager::closeRingingNotification()
{
    _ringingNotification->disconnect();
    _ringingNotification->close();
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

void NotificationManager::handleIncomingCall(const QString &deviceUni, const QString &callUni, const QString &communicationWith)
{
    const QString contactName = _contactUtils->displayString(communicationWith);

    bool allowed = true;

    if (Config::self()->adaptiveBlocking() && contactName == communicationWith) {
        allowed = false;

        if (Config::self()->allowAnonymous() && communicationWith.isEmpty()) {
            allowed = true;
        }

        if (Config::self()->allowPreviousOutgoing()) {
            QString lastOutgoing = _databaseInterface->lastCall(communicationWith, static_cast<int>(DialerTypes::CallDirection::Outgoing));
            if (!lastOutgoing.isEmpty()) {
                allowed = true;
            }
        }

        if (Config::self()->allowCallback()) {
            QString lastIncoming = _databaseInterface->lastCall(communicationWith, static_cast<int>(DialerTypes::CallDirection::Incoming));
            QDateTime lastTime = QDateTime::fromString(lastIncoming, QStringLiteral("yyyy-MM-ddThh:mm:ss.zzz"));
            qint64 diff = lastTime.msecsTo(QDateTime::currentDateTime());
            if (diff / 1000 / 60 < Config::self()->callbackInterval()) {
                allowed = true;
            }
        }

        if (Config::self()->allowedPatterns().length() > 0) {
            for (const auto &pattern : Config::self()->allowedPatterns()) {
                if (communicationWith.indexOf(pattern) >= 0) {
                    allowed = true;
                    break;
                }
            }
        }
    }

    QString notificationEvent = QStringLiteral("ringing");

    bool silent = false;
    if (!allowed) {
        if (Config::self()->ringOption() == 0) {
            hangUp(deviceUni, callUni);
            return;
        } else if (Config::self()->ringOption() == 1) {
            return;
        }
        notificationEvent = QStringLiteral("ringing-silent");
        silent = true;
    }

    bool screenLocked = getScreenSaverActive();

    bool skipNotification = false;
    if (screenLocked) {
        notificationEvent = QStringLiteral("ringing-without-popup");
        if (silent) {
            skipNotification = true;
        }
    }

    if (allowed) {
        startHapticsFeedback();
    }

    QString callerDisplay =
        (contactName == communicationWith) ? communicationWith : communicationWith + QStringLiteral("<br>") + QStringLiteral("<b>%1</b>").arg(contactName);

    if (callerDisplay.isEmpty()) {
        callerDisplay = i18n("No Caller ID");
    }

    if (!skipNotification) {
        openRingingNotification(deviceUni, callUni, callerDisplay, notificationEvent);
    }

    if (screenLocked) {
        launchPlasmaDialerDesktopFile();
    }
}

void NotificationManager::handleCallInteraction()
{
    stopHapticsFeedback();
    closeRingingNotification();
}

void NotificationManager::startHapticsFeedback()
{
#ifdef HAVE_QT5_FEEDBACK
    _ringEffect->start();
#endif // HAVE_QT5_FEEDBACK
}

void NotificationManager::stopHapticsFeedback()
{
#ifdef HAVE_QT5_FEEDBACK
    _ringEffect->stop();
#endif // HAVE_QT5_FEEDBACK
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
        launchPlasmaDialerDesktopFile();
        break;
    case 2:
        hangUp(deviceUni, callUni);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}
