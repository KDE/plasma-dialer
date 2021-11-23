// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-manager.h"
#include "dialer-audio.h"

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>

#include <QDebug>

static void enable_earpiece()
{
    DialerAudio::instance()->setCallMode(CallActive, AudioModeEarpiece);
}

static void enable_normal()
{
    DialerAudio::instance()->setCallMode(CallEnded, AudioModeWiredOrSpeaker);
}

static void enable_speaker()
{
    DialerAudio::instance()->setCallMode(CallActive, AudioModeSpeaker);
}

DialerManager::DialerManager(QObject *parent)
    : QObject(parent)
{
}

DialerManager::~DialerManager()
{
    enable_normal();
    qDebug() << "Deleting DialerManager";
}

void DialerManager::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
}

void DialerManager::setContactUtils(ContactUtils *contactUtils)
{
    _contactUtils = contactUtils;
}

void DialerManager::setDialerUtils(DialerUtils *dialerUtils)
{
    qDebug() << Q_FUNC_INFO;
    _dialerUtils = dialerUtils;
    DialerAudio::instance();

    connect(_dialerUtils, &DialerUtils::speakerModeChanged, this, &DialerManager::onSetSpeakerModeRequested);
    connect(_dialerUtils, &DialerUtils::muteChanged, this, &DialerManager::onSetMuteRequested);
}

void DialerManager::onCallStateChanged(const QString &deviceUni,
                                       const QString &callUni,
                                       const DialerTypes::CallDirection &callDirection,
                                       const DialerTypes::CallState &callState,
                                       const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << "new call state:" << callDirection;
    if (!_contactUtils) {
        qCritical() << Q_FUNC_INFO;
    }
    switch (callState) {
    case DialerTypes::CallState::Dialing:
        enable_earpiece();
        break;
    case DialerTypes::CallState::Terminated:
        enable_normal();
    default:
        enable_earpiece();
    }
}

void DialerManager::onSetSpeakerModeRequested(bool enabled)
{
    if (enabled) {
        enable_speaker();
    } else {
        enable_earpiece();
    }
}

void DialerManager::onSetMuteRequested(bool muted)
{
    DialerAudio::instance()->setMicMute(muted);
}
