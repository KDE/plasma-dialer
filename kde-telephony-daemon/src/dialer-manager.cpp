// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-FileCopyrightText: 2022 Bhushan Shah <bshah@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-manager.h"
#include "dialer-utils.h"

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>

#include <QDebug>

#include "dialer-audio.h"
#include "mprisplayerinterface.h"

DialerManager::DialerManager(QObject *parent)
    : QObject(parent)
{
    DialerAudio::instance();
}

DialerManager::~DialerManager()
{
}

void DialerManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    m_callUtils = callUtils;

    connect(m_callUtils, &org::kde::telephony::CallUtils::callAdded, this, &DialerManager::onUtilsCallAdded);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callsChanged, this, &DialerManager::onUtilsCallsChanged);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &DialerManager::onUtilsCallStateChanged);
}

void DialerManager::setDialerUtils(DialerUtils *dialerUtils)
{
    qDebug() << Q_FUNC_INFO;
    m_dialerUtils = dialerUtils;

    connect(m_dialerUtils, &DialerUtils::muteChanged, this, &DialerManager::onUtilsMuteChanged);
    connect(m_dialerUtils, &DialerUtils::speakerModeChanged, this, &DialerManager::onUtilsSpeakerModeChanged);

    connect(m_dialerUtils, &DialerUtils::muteRequested, this, &DialerManager::onUtilsMuteRequested);
    connect(m_dialerUtils, &DialerUtils::speakerModeRequested, this, &DialerManager::onUtilsSpeakerModeRequested);

    m_dialerUtils->fetchMute();
    m_dialerUtils->fetchSpeakerMode();
}

void DialerManager::onUtilsCallAdded(const QString &deviceUni,
                                     const QString &callUni,
                                     const DialerTypes::CallDirection &callDirection,
                                     const DialerTypes::CallState &callState,
                                     const DialerTypes::CallStateReason &callStateReason,
                                     const QString communicationWith)
{
    Q_UNUSED(communicationWith);
    if (!m_callUtils) {
        qCritical() << Q_FUNC_INFO;
    }
    qDebug() << Q_FUNC_INFO << "call added" << deviceUni << callUni << callDirection << callState << callStateReason;

    if (callDirection == DialerTypes::CallDirection::Incoming) {
        if (callState == DialerTypes::CallState::RingingIn) {
            pauseMedia();
        }
    }
}

void DialerManager::onUtilsCallsChanged(const DialerTypes::CallDataVector &calls)
{
    if (calls.isEmpty()) {
        unpauseMedia();
    }
}

void DialerManager::onUtilsCallStateChanged(const DialerTypes::CallData &callData)
{
    if (!m_callUtils) {
        qCritical() << Q_FUNC_INFO;
    }
    qDebug() << Q_FUNC_INFO << "new call state:" << callData.state;
}

// get current speaker mode and pass it to the DailerUtils
void DialerManager::onUtilsSpeakerModeRequested()
{
    bool speakerMode = DialerAudio::instance()->getCallMode() & AudioModeSpeaker;
    m_dialerUtils->setSpeakerMode(speakerMode);
}

// get current mute state and pass it to the DialerUtils
void DialerManager::onUtilsMuteRequested()
{
    auto micMute = DialerAudio::instance()->getMicMute();
    m_dialerUtils->setMute(micMute);
}

// set speaker mode
void DialerManager::onUtilsSpeakerModeChanged(bool enabled)
{
    if (enabled) {
        DialerAudio::instance()->setCallMode(CallActive, AudioModeSpeaker);
    } else {
        DialerAudio::instance()->setCallMode(CallActive, AudioModeEarpiece);
    }
}

// set mute mode
void DialerManager::onUtilsMuteChanged(bool muted)
{
    DialerAudio::instance()->setMicMute(muted);
}

void DialerManager::pauseMedia()
{
    auto sessionBus = QDBusConnection::sessionBus();
    const QStringList interfaces = sessionBus.interface()->registeredServiceNames().value();
    for (const QString &iface : interfaces) {
        if (iface.startsWith(QLatin1String("org.mpris.MediaPlayer2"))) {
            if (iface.contains(QLatin1String("kdeconnect"))) {
                qDebug() << Q_FUNC_INFO << "Skip players connected over KDE Connect. KDE Connect pauses them itself";
                continue;
            }
            qDebug() << Q_FUNC_INFO << "Found player:" << iface;

            org::mpris::MediaPlayer2::Player mprisInterface(iface, QStringLiteral("/org/mpris/MediaPlayer2"), sessionBus);
            QString status = mprisInterface.playbackStatus();
            qDebug() << Q_FUNC_INFO << "Found player status:" << iface << status;
            if (status == QLatin1String("Playing")) {
                if (!m_pausedSources.contains(iface)) {
                    m_pausedSources.insert(iface);
                    if (mprisInterface.canPause()) {
                        mprisInterface.Pause();
                    } else {
                        mprisInterface.Stop();
                    }
                }
            }
        }
    }
}

void DialerManager::unpauseMedia()
{
    auto sessionBus = QDBusConnection::sessionBus();
    for (const QString &iface : std::as_const(m_pausedSources)) {
        org::mpris::MediaPlayer2::Player mprisInterface(iface, QStringLiteral("/org/mpris/MediaPlayer2"), sessionBus);
        mprisInterface.Play();
    }
    m_pausedSources.clear();
}
