// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-FileCopyrightText: 2022 Bhushan Shah <bshah@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-manager.h"

#include <glib.h>

#include <libcallaudio-enums.h>
#include <libcallaudio.h>

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>

#include <QDebug>

static void enable_callmode()
{
    GError *err = nullptr;
    if (!call_audio_select_mode(CALL_AUDIO_MODE_CALL, &err)) {
        qWarning() << "Failed to set call mode to earpiece";
    }
}

static void enable_normal()
{
    GError *err = nullptr;
    if (!call_audio_select_mode(CALL_AUDIO_MODE_DEFAULT, &err)) {
        qWarning() << "Failed to set default callaudio mode";
    }
}

DialerManager::DialerManager(QObject *parent)
    : QObject(parent)
{
    GError *err = nullptr;
    if (!call_audio_init(&err)) {
        qWarning() << "Failed to init callaudio" << err->message;
    }
}

DialerManager::~DialerManager()
{
    enable_normal();
    call_audio_deinit();
    qDebug() << "Deleting DialerManager";
}

void DialerManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    _callUtils = callUtils;

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
    case DialerTypes::CallState::RingingOut:
        enable_callmode();
        break;
    case DialerTypes::CallState::Terminated:
        enable_normal();
        break;
    default:
        break;
    }
}

void DialerManager::onSpeakerModeFetched()
{
    bool speakerMode = call_audio_get_speaker_state() == CALL_AUDIO_SPEAKER_ON;
    Q_EMIT _dialerUtils->speakerModeChanged(speakerMode);
}

void DialerManager::onMuteFetched()
{
    auto micMute = call_audio_get_mic_state() == CALL_AUDIO_MIC_OFF;
    Q_EMIT _dialerUtils->muteChanged(micMute);
}

void DialerManager::onSetSpeakerModeRequested(bool enabled)
{
    GError *err = nullptr;
    if (!call_audio_enable_speaker(enabled, &err)) {
        qWarning() << "Failed to set speaker mode" << enabled;
    }
}

void DialerManager::onSetMuteRequested(bool muted)
{
    GError *err = nullptr;
    if (!call_audio_mute_mic(muted, &err)) {
        qWarning() << "Failed to set mute mode" << muted;
    }
}
