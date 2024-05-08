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
    , m_ringingNotification(std::make_unique<KNotification>(QStringLiteral("ringing"), KNotification::Persistent | KNotification::LoopSound, this))
    , m_callStarted(false)

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

    m_ringingNotification->setAutoDelete(false);
}

void NotificationManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    qDebug() << Q_FUNC_INFO;
    m_callUtils = callUtils;

    connect(m_callUtils, &org::kde::telephony::CallUtils::callAdded, this, &NotificationManager::onCallAdded);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &NotificationManager::onCallStateChanged);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callDeleted, this, &NotificationManager::onCallDeleted);
}

void NotificationManager::setContactUtils(ContactUtils *contactUtils)
{
    m_contactUtils = contactUtils;
}

void NotificationManager::setCallHistoryDatabase(CallHistoryDatabase *callHistoryDatabase)
{
    m_callHistoryDatabase = callHistoryDatabase;
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
            m_callStarted = false;
        }
    }
}

void NotificationManager::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    qDebug() << Q_FUNC_INFO << "call deleted" << deviceUni << callUni;
    handleCallInteraction();
}

void NotificationManager::onCallStateChanged(const DialerTypes::CallData &callData)
{
    qDebug() << Q_FUNC_INFO << "call state changed:" << callData.state << callData.stateReason;

    const QString contactName = m_contactUtils->displayString(callData.communicationWith);
    QString callerDisplay = (contactName == callData.communicationWith)
        ? callData.communicationWith
        : callData.communicationWith + QStringLiteral("<br>") + QStringLiteral("<b>%1</b>").arg(contactName);
    if (callerDisplay.isEmpty()) {
        callerDisplay = i18n("No Caller ID");
    }

    if (callData.direction == DialerTypes::CallDirection::Incoming) {
        if (callData.state == DialerTypes::CallState::Terminated) {
            handleCallInteraction();

            if (callData.stateReason == DialerTypes::CallStateReason::Unknown && !m_callStarted) {
                auto missedCallNotification = new KNotification(QStringLiteral("callMissed"), KNotification::Persistent, this);
                missedCallNotification->setComponentName(QStringLiteral("plasma_dialer"));
                missedCallNotification->setTitle(i18n("Missed call"));
                missedCallNotification->setText(i18n("Missed call from %1", callerDisplay));
                missedCallNotification->sendEvent();
            }
        }
        if (callData.state == DialerTypes::CallState::Active) {
            handleCallInteraction();
            m_callStarted = true;
        }
    }
}

void NotificationManager::openRingingNotification(const QString &deviceUni,
                                                  const QString &callUni,
                                                  const QString callerDisplay,
                                                  const QString notificationEvent)
{
    m_ringingNotification->clearActions();
    m_ringingNotification->setEventId(notificationEvent);
    m_ringingNotification->setUrgency(KNotification::CriticalUrgency);
    m_ringingNotification->setComponentName(QStringLiteral("plasma_dialer"));

    // _ringingNotification->setPixmap(person.photo());
    m_ringingNotification->setTitle(i18n("Incoming call"));
    m_ringingNotification->setText(callerDisplay);
    // this will be used by the notification applet to show custom notification UI
    // with swipe decision.
    m_ringingNotification->setHint(QStringLiteral("category"), QStringLiteral("x-kde.incoming-call"));

    auto acceptAction = m_ringingNotification->addAction(i18n("Accept"));
    connect(acceptAction, &KNotificationAction::activated, this, [this, deviceUni, callUni] {
        accept(deviceUni, callUni);
        launchPlasmaDialerDesktopFile();
    });

    auto rejectAction = m_ringingNotification->addAction(i18n("Reject"));
    connect(rejectAction, &KNotificationAction::activated, this, [this, deviceUni, callUni] {
        hangUp(deviceUni, callUni);
    });

    m_ringingNotification->sendEvent();
}

void NotificationManager::closeRingingNotification()
{
    m_ringingNotification->disconnect();
    m_ringingNotification->close();
}

void NotificationManager::accept(const QString &deviceUni, const QString &callUni)
{
    QDBusPendingReply<> reply = m_callUtils->accept(deviceUni, callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    closeRingingNotification();
}

void NotificationManager::hangUp(const QString &deviceUni, const QString &callUni)
{
    QDBusPendingReply<> reply = m_callUtils->hangUp(deviceUni, callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    closeRingingNotification();
}

void NotificationManager::handleIncomingCall(const QString &deviceUni, const QString &callUni, const QString &communicationWith)
{
    const QString contactName = m_contactUtils->displayString(communicationWith);

    bool allowed = true;

    if (Config::self()->adaptiveBlocking() && contactName == communicationWith) {
        allowed = false;

        if (Config::self()->allowAnonymous() && communicationWith.isEmpty()) {
            allowed = true;
        }

        if (Config::self()->allowPreviousOutgoing()) {
            QString lastOutgoing = m_callHistoryDatabase->lastCall(communicationWith, static_cast<int>(DialerTypes::CallDirection::Outgoing));
            if (!lastOutgoing.isEmpty()) {
                allowed = true;
            }
        }

        if (Config::self()->allowCallback()) {
            QString lastIncoming = m_callHistoryDatabase->lastCall(communicationWith, static_cast<int>(DialerTypes::CallDirection::Incoming));
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
