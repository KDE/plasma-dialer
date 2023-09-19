// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include "dialerutilsinterface.h"

class DeclarativeDialerUtils : public org::kde::telephony::DialerUtils
{
    Q_OBJECT
    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(bool speakerMode READ speakerMode WRITE setSpeakerMode NOTIFY speakerModeChanged)

public:
    DeclarativeDialerUtils(QObject *parent = nullptr);
    Q_INVOKABLE void syncSettings();

Q_SIGNALS:
    // empty parameter list is a moc limitation for NOTIFY
    void muteChanged();
    void speakerModeChanged();
};
