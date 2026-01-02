// SPDX-FileCopyrightText: 2021 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <PulseAudioQt/Card>
#include <PulseAudioQt/Context>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/Source>
#include <QObject>

enum AudioMode {
    AudioModeEarpiece = 0x0001,
    AudioModeWiredHeadset = 0x0002,
    AudioModeSpeaker = 0x0004,
    AudioModeWiredOrEarpiece = AudioModeWiredHeadset | AudioModeEarpiece,
    AudioModeWiredOrSpeaker = AudioModeWiredHeadset | AudioModeSpeaker,
};

Q_DECLARE_METATYPE(AudioMode)

typedef QList<AudioMode> AudioModes;
Q_DECLARE_METATYPE(AudioModes)

enum CallStatus {
    CallRinging,
    CallActive,
    CallEnded
};

Q_DECLARE_METATYPE(CallStatus)

class DialerAudio : public QObject
{
    Q_OBJECT
public:
    explicit DialerAudio(QObject *parent = nullptr);
    ~DialerAudio() override;
    static DialerAudio *instance();

    AudioMode getCallMode();
    bool getMicMute();

    void setCallMode(CallStatus callstatus, AudioMode audiomode);
    void setMicMute(bool muted);

private:
    void cardAdded(PulseAudioQt::Card *card);

    void savePulseState();

    PulseAudioQt::Card *_voiceCallCard = nullptr;
    PulseAudioQt::Profile *_voiceCallProfile = nullptr;
    PulseAudioQt::Profile *_previousProfile = nullptr;

    CallStatus _callStatus = CallEnded;
    AudioMode _audioMode = AudioModeWiredOrSpeaker;
    bool _micMuted = false;
    qint64 _prevVolume;
};
