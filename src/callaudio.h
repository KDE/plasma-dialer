/*
    SPDX-FileCopyrightText: 2021 Bhushan Shah <bshah@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CALLAUDIO_H
#define CALLAUDIO_H

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

enum CallStatus { CallRinging, CallActive, CallEnded };

Q_DECLARE_METATYPE(CallStatus)

class CallAudio : public QObject
{
    Q_OBJECT
public:
    explicit CallAudio(QObject *parent = nullptr);
    ~CallAudio() override;
    static CallAudio *instance();

    void setCallMode(CallStatus callstatus, AudioMode audiomode);
    void setMicMute(bool muted);

private:
    void cardAdded(PulseAudioQt::Card *card);

    void savePulseState();

    PulseAudioQt::Card *m_voiceCallCard = nullptr;
    PulseAudioQt::Profile *m_voiceCallProfile = nullptr;
    PulseAudioQt::Profile *m_previousProfile = nullptr;

    CallStatus m_callStatus = CallEnded;
    AudioMode m_audioMode = AudioModeWiredOrSpeaker;
    bool m_micMuted = false;
    quint64 m_prevVolume;
};

#endif
