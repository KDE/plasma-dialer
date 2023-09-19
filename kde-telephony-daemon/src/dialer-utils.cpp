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
    return _mute;
}

bool DialerUtils::speakerMode()
{
    return _speakerMode;
}

void DialerUtils::setSpeakerMode(bool enabled)
{
    if (_speakerMode != enabled) {
        _speakerMode = enabled;
        Q_EMIT speakerModeChanged(_speakerMode);
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
    if (_mute != muted) {
        _mute = muted;
        Q_EMIT muteChanged(_mute);
    }
}

void DialerUtils::syncSettings()
{
    Config::self()->load();
}
