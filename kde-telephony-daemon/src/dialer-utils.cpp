// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-utils.h"

#include <QDebug>

#include "config.h"

DialerUtils::DialerUtils(QObject *parent)
    : QObject(parent)
{
}

bool DialerUtils::mute()
{
    return m_mute;
}

bool DialerUtils::speakerMode()
{
    return m_speakerMode;
}

void DialerUtils::setSpeakerMode(bool enabled)
{
    if (m_speakerMode != enabled) {
        m_speakerMode = enabled;
        Q_EMIT speakerModeChanged(m_speakerMode);
    }
}

void DialerUtils::fetchMute()
{
    Q_EMIT muteRequested();
}

void DialerUtils::fetchSpeakerMode()
{
    Q_EMIT speakerModeRequested();
}

void DialerUtils::setMute(bool muted)
{
    if (m_mute != muted) {
        m_mute = muted;
        Q_EMIT muteChanged(m_mute);
    }
}

void DialerUtils::syncSettings()
{
    Config::self()->load();
}
