// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include "dialerutilsinterface.h"

class DeclarativeDialerUtils : public org::kde::telephony::DialerUtils
{
    Q_OBJECT
    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY declarativeMuteChanged)
    Q_PROPERTY(bool speakerMode READ speakerMode WRITE setSpeakerMode NOTIFY declarativeSpeakerModeChanged)

public:
    DeclarativeDialerUtils(QObject *parent = nullptr);

    Q_INVOKABLE void setSpeakerMode(bool enabled);
    Q_INVOKABLE void setMute(bool muted);
    bool mute() const;
    bool speakerMode() const;
    Q_INVOKABLE void syncSettings();

Q_SIGNALS:
    void declarativeMuteChanged(bool mute);
    void declarativeSpeakerModeChanged(bool speakerMode);

private Q_SLOTS:
    void _onMuteChanged(bool mute);
    void _onSpeakerModeChanged(bool speakerMode);

private:
    bool _mute;
    bool _speakerMode;

    void _fetchMute();
    void _fetchSpeakerMode();
};
