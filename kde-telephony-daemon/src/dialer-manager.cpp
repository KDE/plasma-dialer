// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-FileCopyrightText: 2022 Bhushan Shah <bshah@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-manager.h"

#include <glib.h>

#ifndef LOWER_LIBCALLAUDIO_VERSION
#include <libcallaudio-enums.h>
#endif // LOWER_LIBCALLAUDIO_VERSION
#include <libcallaudio.h>

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>

#include <QDebug>

#include "mprisplayerinterface.h"

static void enable_call_mode()
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#endif // LOWER_LIBCALLAUDIO_VERSION
    call_audio_select_mode_async(CALL_AUDIO_MODE_CALL, nullptr, nullptr);
}

static void enable_default_mode()
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#endif // LOWER_LIBCALLAUDIO_VERSION
    call_audio_select_mode_async(CALL_AUDIO_MODE_DEFAULT, nullptr, nullptr);
}

static void mute_mic(bool want_mute)
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#endif // LOWER_LIBCALLAUDIO_VERSION
    call_audio_mute_mic_async(want_mute, nullptr, nullptr);
}

static void enable_speaker(bool want_speaker)
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#endif // LOWER_LIBCALLAUDIO_VERSION
    call_audio_enable_speaker_async(want_speaker, nullptr, nullptr);
}

DialerManager::DialerManager(QObject *parent)
    : QObject(parent)
    , m_needsDefaultAudioMode(false)
{
    GError *err = nullptr;
    if (!call_audio_init(&err)) {
        qWarning() << "Failed to init callaudio" << err->message;
    }
}

DialerManager::~DialerManager()
{
    enable_default_mode();
    call_audio_deinit();
    qDebug() << "Deleting DialerManager";
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
    switch (callData.state) {
    case DialerTypes::CallState::Active:
        enable_call_mode();
        m_needsDefaultAudioMode = true;
        break;
    case DialerTypes::CallState::Terminated:
        if (m_needsDefaultAudioMode) {
            enable_default_mode();
            m_needsDefaultAudioMode = false;
        }

        break;
    default:
        break;
    }
}

void DialerManager::onUtilsSpeakerModeRequested()
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#else // LOWER_LIBCALLAUDIO_VERSION
    bool speakerMode = call_audio_get_speaker_state() == CALL_AUDIO_SPEAKER_ON;
    m_dialerUtils->setSpeakerMode(speakerMode);
#endif // LOWER_LIBCALLAUDIO_VERSION
}

void DialerManager::onUtilsMuteRequested()
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#else // LOWER_LIBCALLAUDIO_VERSION
    auto micMute = call_audio_get_mic_state() == CALL_AUDIO_MIC_OFF;
    m_dialerUtils->setMute(micMute);
#endif // LOWER_LIBCALLAUDIO_VERSION
}

void DialerManager::onUtilsSpeakerModeChanged(bool enabled)
{
    enable_speaker(enabled);
}

void DialerManager::onUtilsMuteChanged(bool muted)
{
    mute_mic(muted);
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
    for (const QString &iface : qAsConst(m_pausedSources)) {
        org::mpris::MediaPlayer2::Player mprisInterface(iface, QStringLiteral("/org/mpris/MediaPlayer2"), sessionBus);
        mprisInterface.Play();
    }
    m_pausedSources.clear();
}
