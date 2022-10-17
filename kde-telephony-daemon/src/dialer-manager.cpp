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
    , _needsDefaultAudioMode(false)
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
    _callUtils = callUtils;

    connect(_callUtils, &org::kde::telephony::CallUtils::callAdded, this, &DialerManager::onCallAdded);
    connect(_callUtils, &org::kde::telephony::CallUtils::fetchedCallsChanged, this, &DialerManager::onFetchedCallsChanged);
    connect(_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &DialerManager::onCallStateChanged);
}

void DialerManager::setDialerUtils(DialerUtils *dialerUtils)
{
    qDebug() << Q_FUNC_INFO;
    _dialerUtils = dialerUtils;

    connect(_dialerUtils, &DialerUtils::speakerModeChanged, this, &DialerManager::onSetSpeakerModeRequested);
    connect(_dialerUtils, &DialerUtils::muteChanged, this, &DialerManager::onSetMuteRequested);

    connect(_dialerUtils, &DialerUtils::speakerModeFetched, this, &DialerManager::onSpeakerModeFetched);
    connect(_dialerUtils, &DialerUtils::muteFetched, this, &DialerManager::onMuteFetched);
}

void DialerManager::onCallAdded(const QString &deviceUni,
                                const QString &callUni,
                                const DialerTypes::CallDirection &callDirection,
                                const DialerTypes::CallState &callState,
                                const DialerTypes::CallStateReason &callStateReason,
                                const QString communicationWith)
{
    if (!_callUtils) {
        qCritical() << Q_FUNC_INFO;
    }
    qDebug() << Q_FUNC_INFO << "call added" << deviceUni << callUni << callDirection << callState << callStateReason;

    if (callDirection == DialerTypes::CallDirection::Incoming) {
        if (callState == DialerTypes::CallState::RingingIn) {
            pauseMedia();
        }
    }
}

void DialerManager::onFetchedCallsChanged(const DialerTypes::CallDataVector &fetchedCalls)
{
    if (fetchedCalls.isEmpty()) {
        unpauseMedia();
    }
}

void DialerManager::onCallStateChanged(const QString &deviceUni,
                                       const QString &callUni,
                                       const DialerTypes::CallDirection &callDirection,
                                       const DialerTypes::CallState &callState,
                                       const DialerTypes::CallStateReason &callStateReason)
{
    Q_UNUSED(deviceUni)
    Q_UNUSED(callUni)
    Q_UNUSED(callDirection)
    Q_UNUSED(callStateReason)

    if (!_callUtils) {
        qCritical() << Q_FUNC_INFO;
    }
    qDebug() << Q_FUNC_INFO << "new call state:" << callState;
    switch (callState) {
    case DialerTypes::CallState::Active:
        enable_call_mode();
        _needsDefaultAudioMode = true;
        break;
    case DialerTypes::CallState::Terminated:
        if (_needsDefaultAudioMode) {
            enable_default_mode();
            _needsDefaultAudioMode = false;
        }

        break;
    default:
        break;
    }
}

void DialerManager::onSpeakerModeFetched()
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#else // LOWER_LIBCALLAUDIO_VERSION
    bool speakerMode = call_audio_get_speaker_state() == CALL_AUDIO_SPEAKER_ON;
    Q_EMIT _dialerUtils->speakerModeChanged(speakerMode);
#endif // LOWER_LIBCALLAUDIO_VERSION
}

void DialerManager::onMuteFetched()
{
#ifdef LOWER_LIBCALLAUDIO_VERSION
    qWarning() << "callaudio version is not supported";
    return;
#else // LOWER_LIBCALLAUDIO_VERSION
    auto micMute = call_audio_get_mic_state() == CALL_AUDIO_MIC_OFF;
    Q_EMIT _dialerUtils->muteChanged(micMute);
#endif // LOWER_LIBCALLAUDIO_VERSION
}

void DialerManager::onSetSpeakerModeRequested(bool enabled)
{
    enable_speaker(enabled);
}

void DialerManager::onSetMuteRequested(bool muted)
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
                if (!_pausedSources.contains(iface)) {
                    _pausedSources.insert(iface);
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
    for (const QString &iface : qAsConst(_pausedSources)) {
        org::mpris::MediaPlayer2::Player mprisInterface(iface, QStringLiteral("/org/mpris/MediaPlayer2"), sessionBus);
        mprisInterface.Play();
    }
    _pausedSources.clear();
}
