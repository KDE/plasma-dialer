// SPDX-FileCopyrightText: 2021 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDebug>

#include <PulseAudioQt/Card>
#include <PulseAudioQt/CardPort>
#include <PulseAudioQt/Context>
#include <PulseAudioQt/Profile>
#include <PulseAudioQt/Source>

#include <optional>

#include "dialer-audio.h"

DialerAudio::DialerAudio(QObject *parent)
    : QObject(parent)
{
    if (!PulseAudioQt::Context::instance()->isValid()) {
        qDebug() << "Pulseaudio context is not valid";
        return;
    }

    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::cardAdded, this, &DialerAudio::cardAdded);
}

void DialerAudio::cardAdded(PulseAudioQt::Card *card)
{
    if (_voiceCallCard) {
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

    _voiceCallCard = card;
    _voiceCallProfile = *voiceCallProfile;
}

void DialerAudio::setCallMode(CallStatus callStatus, AudioMode audioMode)
{
    // first record previous state
    CallStatus prevStatus = _callStatus;
    AudioMode prevMode = _audioMode;

    // if we are transitioning to active/ringing call status, we need to save pulseaudio state
    if (prevStatus == CallEnded && callStatus != CallEnded) {
        // save pulseaudio state
        qDebug() << "saving pulseaudio state";
        savePulseState();
    }

    // save new state
    _callStatus = callStatus;
    _audioMode = audioMode;

    if (!_voiceCallCard) {
        qWarning() << "Skipping audio setup because no voice compatible card found";
        return;
    }

    // now if we have a active call then switch profile to the VoiceCall profile
    quint32 voiceCardIndex = PulseAudioQt::Context::instance()->cards().indexOf(_voiceCallCard);
    if ((_callStatus == CallActive) && (prevStatus != CallActive) && _voiceCallCard && _voiceCallProfile) {
        qDebug() << "Setting current profile to " << _voiceCallProfile->name();
        PulseAudioQt::Context::instance()->setCardProfile(voiceCardIndex, _voiceCallProfile->name());
    } else if ((_callStatus == CallEnded) && (prevStatus != CallEnded) && _voiceCallCard && _previousProfile) {
        qDebug() << "Setting current profile to " << _previousProfile->name();
        PulseAudioQt::Context::instance()->setCardProfile(voiceCardIndex, _previousProfile->name());
    }

    // now we figure out the audio sink/source switching part
    // this works in two steps, first we query active source/sink
    // and then depending on requested mode, we set the active port for both source and sink

    const auto sources = _voiceCallCard->sources();
    const auto sinks = _voiceCallCard->sinks();
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
    if (_audioMode & AudioModeEarpiece) {
        if (builtinMicIndex.has_value()) {
            preferredSourcePort = builtinMicIndex.value();
        }
        if (outputEarpieceIndex.has_value()) {
            preferredSinkPort = outputEarpieceIndex.value();
        }
    }
    if (_audioMode & AudioModeSpeaker) {
        if (builtinMicIndex.has_value()) {
            preferredSourcePort = builtinMicIndex.value();
        }
        if (outputSpeakerIndex.has_value()) {
            preferredSinkPort = outputSpeakerIndex.value();
        }
    }
    if ((_audioMode & AudioModeWiredHeadset)) {
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

void DialerAudio::savePulseState()
{
    if (!_voiceCallCard) {
        return;
    }
    quint32 prevIndex = _voiceCallCard->activeProfileIndex();
    _previousProfile = _voiceCallCard->profiles().at(prevIndex);
}

void DialerAudio::setMicMute(bool muted)
{
    if (!_voiceCallCard) {
        return;
    }
    _micMuted = muted;

    if (_callStatus == CallEnded)
        return;
    const auto sources = _voiceCallCard->sources();
    PulseAudioQt::Source *activeCardSource = sources.first();
    if (_micMuted) {
        _prevVolume = activeCardSource->volume();
        activeCardSource->setVolume(0);
        activeCardSource->setMuted(_micMuted);
    } else {
        activeCardSource->setMuted(_micMuted);
        activeCardSource->setVolume(_prevVolume);
    }
}

Q_GLOBAL_STATIC(DialerAudio, dialerAudio)

DialerAudio *DialerAudio::instance()
{
    DialerAudio *audio = dialerAudio();
    return audio;
}

DialerAudio::~DialerAudio() = default;
