// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-utils.h"

#include <QDebug>

#include "phonenumbers/asyoutypeformatter.h"
#include "phonenumbers/phonenumberutil.h"

DialerUtils::DialerUtils(QObject *parent)
    : QObject(parent)
{
}

void DialerUtils::setSpeakerMode(bool enabled)
{
    Q_EMIT speakerModeChanged(enabled);
}

void DialerUtils::setMute(bool muted)
{
    Q_EMIT muteChanged(muted);
}
