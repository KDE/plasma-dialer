/*
    SPDX-FileCopyrightText: 2021 Bhushan Shah <bshah@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QDebug>

#include <PulseAudioQt/Card>
#include <PulseAudioQt/CardPort>
#include <PulseAudioQt/Context>
#include <PulseAudioQt/Profile>
#include <PulseAudioQt/Source>

#include <optional>

#include "callaudio.h"

CallAudio::CallAudio(QObject *parent)
    : QObject(parent)
{
    if (!PulseAudioQt::Context::instance()->isValid()) {
        qDebug() << "Pulseaudio context is not valid";
        return;
    }

    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::cardAdded, this, &CallAudio::cardAdded);
}

void CallAudio::cardAdded(PulseAudioQt::Card *card)
{
    if (m_voiceCallCard) {
        qWarning() << "We already found voicecall compatible card";
        return;
    }

    // first check if we have a voicecall profile in card?
    const auto profiles = card->profiles();
    auto voiceCallProfile = std::find_if(profiles.constBegin(), profiles.constEnd(), [](PulseAudioQt::Profile *profile) {
        return profile->name() == QLatin1String("VoiceCall") || profile->name() == QLatin1String("Voice Call");
    });

    if (voiceCallProfile == profiles.constEnd()) {
        qDebug() << "Card" << card->name() << "is not voice call capable, no VoiceCall profile found";
        return;
    }

    // next check if we have at least earpiece and some form of mic, otherwise this is not usable for the calling
    bool builtinMic = false;
    bool headsetMic = false;
    bool outputSpeaker = false;
    bool outputEarpiece = false;
    bool outputHeadPhone = false;

    const auto ports = card->ports();
    for (const auto port : ports) {
        qDebug() << "Found card port " << port->name();
        builtinMic |= port->type() == PulseAudioQt::Port::Mic;
        headsetMic |= port->type() == PulseAudioQt::Port::Headset;
        outputSpeaker |= port->type() == PulseAudioQt::Port::Speaker;
        outputEarpiece |= port->type() == PulseAudioQt::Port::Earpiece;
        outputHeadPhone |= port->type() == PulseAudioQt::Port::Headphones;
    }

    if (!builtinMic) {
        qDebug() << "Not correct card, no builtin mic" << card->name();
        return;
    }

    if (!outputSpeaker || !outputEarpiece) {
        qDebug() << "Not correct card, no speaker or earpiece found" << card->name();
        return;
    }

    m_voiceCallCard = card;
    m_voiceCallProfile = *voiceCallProfile;
}

void CallAudio::setCallMode(CallStatus callStatus, AudioMode audioMode)
{
    // first record previous state
    CallStatus prevStatus = m_callStatus;
    AudioMode prevMode = m_audioMode;

    // if we are transitioning to active/ringing call status, we need to save pulseaudio state
    if (prevStatus == CallEnded && callStatus != CallEnded) {
        // save pulseaudio state
        qDebug() << "saving pulseaudio state";
        savePulseState();
    }

    // save new state
    m_callStatus = callStatus;
    m_audioMode = audioMode;

    if (!m_voiceCallCard) {
        qWarning() << "Skipping audio setup because no voice compatible card found";
        return;
    }

    // now if we have a active call then switch profile to the VoiceCall profile
    quint32 voiceCardIndex = PulseAudioQt::Context::instance()->cards().indexOf(m_voiceCallCard);
    if ((m_callStatus == CallActive) && (prevStatus != CallActive) && m_voiceCallCard && m_voiceCallProfile) {
        qDebug() << "Setting current profile to " << m_voiceCallProfile->name();
        PulseAudioQt::Context::instance()->setCardProfile(voiceCardIndex, m_voiceCallProfile->name());
    } else if ((m_callStatus == CallEnded) && (prevStatus != CallEnded) && m_voiceCallCard && m_previousProfile) {
        qDebug() << "Setting current profile to " << m_previousProfile->name();
        PulseAudioQt::Context::instance()->setCardProfile(voiceCardIndex, m_previousProfile->name());
    }

    // now we figure out the audio sink/source switching part
    // this works in two steps, first we query active source/sink
    // and then depending on requested mode, we set the active port for both source and sink

    const auto sources = m_voiceCallCard->sources();
    const auto sinks = m_voiceCallCard->sinks();
    PulseAudioQt::Sink *activeCardSink = sinks.first();
    PulseAudioQt::Source *activeCardSource = sources.first();

    // find out the ports for both sources and sinks
    std::optional<quint32> builtinMicIndex, headsetMicIndex;
    const auto sourcePorts = activeCardSource->ports();
    for (const auto port : sourcePorts) {
        if (port->type() == PulseAudioQt::Port::Mic) {
            builtinMicIndex = sourcePorts.indexOf(port);
        }
        if (port->type() == PulseAudioQt::Port::Headset) {
            headsetMicIndex = sourcePorts.indexOf(port);
        }
    }

    std::optional<quint32> outputSpeakerIndex, outputEarpieceIndex, outputHeadPhoneIndex;
    const auto sinkPorts = activeCardSink->ports();
    for (const auto port : sinkPorts) {
        if (port->type() == PulseAudioQt::Port::Speaker) {
            outputSpeakerIndex = sinkPorts.indexOf(port);
        }
        if (port->type() == PulseAudioQt::Port::Earpiece) {
            outputEarpieceIndex = sinkPorts.indexOf(port);
        }
        if (port->type() == PulseAudioQt::Port::Headphones) {
            outputHeadPhoneIndex = sinkPorts.indexOf(port);
        }
    }

    quint32 preferredSourcePort, preferredSinkPort;
    if (m_audioMode & AudioModeEarpiece) {
        if (builtinMicIndex.has_value()) {
            preferredSourcePort = builtinMicIndex.value();
        }
        if (outputEarpieceIndex.has_value()) {
            preferredSinkPort = outputEarpieceIndex.value();
        }
    }
    if (m_audioMode & AudioModeSpeaker) {
        if (builtinMicIndex.has_value()) {
            preferredSourcePort = builtinMicIndex.value();
        }
        if (outputSpeakerIndex.has_value()) {
            preferredSinkPort = outputSpeakerIndex.value();
        }
    }
    if ((m_audioMode & AudioModeWiredHeadset)) {
        if (headsetMicIndex.has_value()) {
            preferredSourcePort = headsetMicIndex.value();
        }
        if (outputHeadPhoneIndex.has_value()) {
            preferredSinkPort = outputHeadPhoneIndex.value();
        }
    }

    activeCardSink->setActivePortIndex(preferredSinkPort);
    activeCardSource->setActivePortIndex(preferredSourcePort);
}

void CallAudio::savePulseState()
{
    if (!m_voiceCallCard) {
        return;
    }
    quint32 prevIndex = m_voiceCallCard->activeProfileIndex();
    m_previousProfile = m_voiceCallCard->profiles().at(prevIndex);
}

void CallAudio::setMicMute(bool muted)
{
    if (!m_voiceCallCard) {
        return;
    }
    m_micMuted = muted;

    if (m_callStatus == CallEnded)
        return;
    const auto sources = m_voiceCallCard->sources();
    PulseAudioQt::Source *activeCardSource = sources.first();
    if (m_micMuted) {
        m_prevVolume = activeCardSource->volume();
        activeCardSource->setVolume(0);
        activeCardSource->setMuted(m_micMuted);
    } else {
        activeCardSource->setMuted(m_micMuted);
        activeCardSource->setVolume(m_prevVolume);
    }
}

Q_GLOBAL_STATIC(CallAudio, callAudio)

CallAudio *CallAudio::instance()
{
    CallAudio *audio = callAudio();
    return audio;
}

CallAudio::~CallAudio() = default;
