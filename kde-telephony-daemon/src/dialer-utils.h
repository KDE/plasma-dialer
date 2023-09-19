// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>

class DialerUtils : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.DialerUtils")

public:
    DialerUtils(QObject *parent = nullptr);

public Q_SLOTS:
    bool mute();
    bool speakerMode();
    void setMute(bool muted);
    void setSpeakerMode(bool enabled);
    void fetchMute();
    void fetchSpeakerMode();
    void syncSettings();

Q_SIGNALS:
    void muteChanged(bool muted);
    void speakerModeChanged(bool enabled);
    void muteRequested();
    void speakerModeRequested();

private:
    bool _mute;
    bool _speakerMode;
};
