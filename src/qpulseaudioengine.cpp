/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file was taken from qt5 and modified by 
** David Henningsson <david.henningsson@canonical.com> for usage in 
** telepathy-ofono.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <QtCore/qdebug.h>

#include "qpulseaudioengine.h"
#include <sys/types.h>
#include <unistd.h>

#define PULSEAUDIO_PROFILE_HSP "headset_head_unit"
#define PULSEAUDIO_PROFILE_A2DP "a2dp_sink"

QT_BEGIN_NAMESPACE

static void contextStateCallbackInit(pa_context *context, void *userdata)
{
    Q_UNUSED(context);
    QPulseAudioEngineWorker *pulseEngine = reinterpret_cast<QPulseAudioEngineWorker*>(userdata);
    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void contextStateCallback(pa_context *context, void *userdata)
{
    Q_UNUSED(userdata);
    Q_UNUSED(context);
}

static void success_cb(pa_context *context, int success, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = reinterpret_cast<QPulseAudioEngineWorker*>(userdata);
    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

/* Callbacks used when handling events from PulseAudio */
static void plug_card_cb(pa_context *c, const pa_card_info *info, int isLast, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (isLast != 0 || !pulseEngine || !info) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }
    pulseEngine->plugCardCallback(info);
}

static void update_card_cb(pa_context *c, const pa_card_info *info, int isLast, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (isLast != 0 || !pulseEngine || !info) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }
    pulseEngine->updateCardCallback(info);
}

static void unplug_card_cb(pa_context *c, const pa_card_info *info, int isLast, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (!pulseEngine) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }

    if (info == NULL) {
        /* That means that the card used to query card_info was removed */
        pulseEngine->unplugCardCallback();
    }
}

static void subscribeCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t idx, void *userdata)
{
    /* For card change events (slot plug/unplug and add/remove card) */
    if ((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_CARD) {
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
            QMetaObject::invokeMethod((QPulseAudioEngineWorker *) userdata, "handleCardEvent",
                    Qt::QueuedConnection, Q_ARG(int, PA_SUBSCRIPTION_EVENT_CHANGE), Q_ARG(unsigned int, idx));
        } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
            QMetaObject::invokeMethod((QPulseAudioEngineWorker *) userdata, "handleCardEvent",
                    Qt::QueuedConnection, Q_ARG(int, PA_SUBSCRIPTION_EVENT_NEW), Q_ARG(unsigned int, idx));
        } else if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            QMetaObject::invokeMethod((QPulseAudioEngineWorker *) userdata, "handleCardEvent",
                    Qt::QueuedConnection, Q_ARG(int, PA_SUBSCRIPTION_EVENT_REMOVE), Q_ARG(unsigned int, idx));
        }
    }
}

QPulseAudioEngineWorker::QPulseAudioEngineWorker(QObject *parent)
    : QObject(parent)
    , m_mainLoopApi(0)
    , m_context(0)
    , m_callstatus(CallEnded)
    , m_audiomode(AudioModeSpeaker)
    , m_micmute(false)
    , m_defaultsink("sink.primary")
    , m_defaultsource("source.primary")
    , m_voicecallcard("")
    , m_voicecallhighest("")
    , m_voicecallprofile("")
    , m_bt_hsp("")
    , m_bt_hsp_a2dp("")
    , m_default_bt_card_fallback("")

{
    m_mainLoop = pa_threaded_mainloop_new();
    if (m_mainLoop == 0) {
        qWarning("Unable to create pulseaudio mainloop");
        return;
    }

    if (pa_threaded_mainloop_start(m_mainLoop) != 0) {
        qWarning("Unable to start pulseaudio mainloop");
        pa_threaded_mainloop_free(m_mainLoop);
        m_mainLoop = 0;
        return;
    }

    createPulseContext();
}

bool QPulseAudioEngineWorker::createPulseContext()
{
    bool keepGoing = true;
    bool ok = true;

    if (m_context)
        return true;

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    pa_threaded_mainloop_lock(m_mainLoop);

    m_context = pa_context_new(m_mainLoopApi, QString(QLatin1String("QtmPulseContext:%1")).arg(::getpid()).toLatin1().constData());
    pa_context_set_state_callback(m_context, contextStateCallbackInit, this);

    if (!m_context) {
        qWarning("Unable to create new pulseaudio context");
        pa_threaded_mainloop_unlock(m_mainLoop);
        return false;
    }

    if (pa_context_connect(m_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL) < 0) {
        qWarning("Unable to create a connection to the pulseaudio context");
        pa_threaded_mainloop_unlock(m_mainLoop);
        releasePulseContext();
        return false;
    }

    pa_threaded_mainloop_wait(m_mainLoop);

    while (keepGoing) {
        switch (pa_context_get_state(m_context)) {
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;

            case PA_CONTEXT_READY:
                qDebug("Pulseaudio connection established.");
                keepGoing = false;
                break;

            case PA_CONTEXT_TERMINATED:
                qCritical("Pulseaudio context terminated.");
                keepGoing = false;
                ok = false;
                break;

            case PA_CONTEXT_FAILED:
            default:
                qCritical() << QString("Pulseaudio connection failure: %1").arg(pa_strerror(pa_context_errno(m_context)));
                keepGoing = false;
                ok = false;
        }

        if (keepGoing) {
            pa_threaded_mainloop_wait(m_mainLoop);
        }
    }

    if (ok) {
        pa_context_set_state_callback(m_context, contextStateCallback, this);
        pa_context_set_subscribe_callback(m_context, subscribeCallback, this);
        pa_context_subscribe(m_context, PA_SUBSCRIPTION_MASK_CARD, NULL, this);
    } else {
        if (m_context) {
            pa_context_unref(m_context);
            m_context = 0;
        }
    }

    pa_threaded_mainloop_unlock(m_mainLoop);
    return true;
}


void QPulseAudioEngineWorker::releasePulseContext()
{
    if (m_context) {
        pa_threaded_mainloop_lock(m_mainLoop);
        pa_context_disconnect(m_context);
        pa_context_unref(m_context);
        pa_threaded_mainloop_unlock(m_mainLoop);
        m_context = 0;
    }

}

QPulseAudioEngineWorker::~QPulseAudioEngineWorker()
{
    releasePulseContext();

    if (m_mainLoop) {
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
        m_mainLoop = 0;
    }
}

void QPulseAudioEngineWorker::cardInfoCallback(const pa_card_info *info)
{
    pa_card_profile_info2 *voice_call = NULL, *highest = NULL;
    pa_card_profile_info2 *hsp = NULL, *a2dp = NULL;

    /* For now we only support one card with the voicecall feature */
    for (int i = 0; i < info->n_profiles; i++) {
        if (!highest || info->profiles2[i]->priority > highest->priority)
            highest = info->profiles2[i];
        if (!strcmp(info->profiles2[i]->name, "voicecall") ||
            !strcmp(info->profiles2[i]->name, "Voice Call"))
            voice_call = info->profiles2[i];
        else if (!strcmp(info->profiles2[i]->name, PULSEAUDIO_PROFILE_HSP) &&
                 info->profiles2[i]->available != 0)
            hsp = info->profiles2[i];
        else if (!strcmp(info->profiles2[i]->name, PULSEAUDIO_PROFILE_A2DP) &&
                 info->profiles2[i]->available != 0)
            a2dp = info->profiles2[i];
    }

    /* Record the card that supports voicecall (default one to be used) */
    if (voice_call) {
        qDebug("Found card that supports voicecall: '%s'", info->name);
        m_voicecallcard = info->name;
        m_voicecallhighest = highest->name;
    qDebug("1");
        m_voicecallprofile = voice_call->name;
    qDebug("2");
    }

    /* Handle the use cases needed for bluetooth */
    if (hsp && a2dp) {
        qDebug("Found card that supports hsp and a2dp: '%s'", info->name);
        m_bt_hsp_a2dp = info->name;
    } else if (hsp && (a2dp == NULL)) {
        /* This card only provides the hsp profile */
        qDebug("Found card that supports only hsp: '%s'", info->name);
        m_bt_hsp = info->name;
    }
    qDebug("3");
}

void QPulseAudioEngineWorker::sinkInfoCallback(const pa_sink_info *info)
{
    pa_sink_port_info *earpiece = NULL, *speaker = NULL;
    pa_sink_port_info *wired_headset = NULL, *wired_headphone = NULL;
    pa_sink_port_info *preferred = NULL;
    pa_sink_port_info *bluetooth_sco = NULL;
    pa_sink_port_info *speaker_and_wired_headphone = NULL;
    AudioMode audiomodetoset;
    AudioModes modes;

    for (int i = 0; i < info->n_ports; i++) {
        if (!strcmp(info->ports[i]->name, "output-earpiece") ||
            !strcmp(info->ports[i]->name, "Earpiece"))
            earpiece = info->ports[i];
        else if (!strcmp(info->ports[i]->name, "output-wired_headset") &&
                (info->ports[i]->available != PA_PORT_AVAILABLE_NO))
            wired_headset = info->ports[i];
        else if ((!strcmp(info->ports[i]->name, "output-wired_headphone") ||
                  !strcmp(info->ports[i]->name, "Headphone")) &&
                (info->ports[i]->available != PA_PORT_AVAILABLE_NO))
            wired_headphone = info->ports[i];
        else if (!strcmp(info->ports[i]->name, "output-speaker") ||
                 !strcmp(info->ports[i]->name, "Speaker"))
            speaker = info->ports[i];
        else if (!strcmp(info->ports[i]->name, "output-bluetooth_sco"))
            bluetooth_sco = info->ports[i];
        else if (!strcmp(info->ports[i]->name, "output-speaker+wired_headphone"))
            speaker_and_wired_headphone = info->ports[i];
    }

    if (!earpiece || !speaker)
        return; /* Not the right sink */

    /* Refresh list of available audio modes */
    modes.append(AudioModeEarpiece);
    modes.append(AudioModeSpeaker);
    if (wired_headset || wired_headphone)
        modes.append(AudioModeWiredHeadset);
    if (bluetooth_sco && ((m_bt_hsp != "") || (m_bt_hsp_a2dp != "")))
        modes.append(AudioModeBluetooth);

    /* Check if the requested mode is available (earpiece*/
    if (((m_audiomode == AudioModeWiredHeadset) && !modes.contains(AudioModeWiredHeadset)) ||
            ((m_audiomode == AudioModeBluetooth) && !modes.contains(AudioModeBluetooth)))
        return;

    /* Now to decide which output to be used, depending on the active mode */
    if (m_audiomode & AudioModeEarpiece) {
        preferred = earpiece;
        audiomodetoset = AudioModeEarpiece;
    }
    if (m_audiomode & AudioModeSpeaker) {
        preferred = speaker;
        audiomodetoset = AudioModeSpeaker;
    }
    if ((m_audiomode & AudioModeWiredHeadset) && (modes.contains(AudioModeWiredHeadset))) {
        preferred = wired_headset ? wired_headset : wired_headphone;
        audiomodetoset = AudioModeWiredHeadset;
    }
    if (m_callstatus == CallRinging && speaker_and_wired_headphone) {
        preferred = speaker_and_wired_headphone;
    }
    if ((m_audiomode & AudioModeBluetooth) && (modes.contains(AudioModeBluetooth))) {
        preferred = bluetooth_sco;
        audiomodetoset = AudioModeBluetooth;
    }

    m_audiomode = audiomodetoset;

    m_nametoset = info->name;
    if (info->active_port != preferred)
        m_valuetoset = preferred->name;

    if (modes != m_availableAudioModes)
        m_availableAudioModes = modes;
}

void QPulseAudioEngineWorker::sourceInfoCallback(const pa_source_info *info)
{
    pa_source_port_info *builtin_mic = NULL, *preferred = NULL;
    pa_source_port_info *wired_headset = NULL, *bluetooth_sco = NULL;

    if (info->monitor_of_sink != PA_INVALID_INDEX)
        return;  /* Not the right source */

    for (int i = 0; i < info->n_ports; i++) {
        if (!strcmp(info->ports[i]->name, "input-builtin_mic") ||
            !strcmp(info->ports[i]->name, "DigitalMic"))
            builtin_mic = info->ports[i];
        else if ((!strcmp(info->ports[i]->name, "input-wired_headset") ||
                  !strcmp(info->ports[i]->name, "HeadsetMic")) &&
                (info->ports[i]->available != PA_PORT_AVAILABLE_NO))
            wired_headset = info->ports[i];
        else if (!strcmp(info->ports[i]->name, "input-bluetooth_sco_headset"))
            bluetooth_sco = info->ports[i];
    }

    if (!builtin_mic)
        return; /* Not the right source */

    /* Now to decide which output to be used, depending on the active mode */
    if ((m_audiomode & AudioModeEarpiece) || (m_audiomode & AudioModeSpeaker))
        preferred = builtin_mic;
    if ((m_audiomode & AudioModeWiredHeadset) && (m_availableAudioModes.contains(AudioModeWiredHeadset)))
        preferred = wired_headset ? wired_headset : builtin_mic;
    if ((m_audiomode & AudioModeBluetooth) && (m_availableAudioModes.contains(AudioModeBluetooth)))
        preferred = bluetooth_sco;

    m_nametoset = info->name;
    if (info->active_port != preferred)
        m_valuetoset = preferred->name;
}

void QPulseAudioEngineWorker::serverInfoCallback(const pa_server_info *info)
{
    /* Saving default sink/source to restore after call hangup */
    m_defaultsink = info->default_sink_name;
    m_defaultsource = info->default_source_name;

    /* In the case of a server callback we need to signal the mainloop */
    pa_threaded_mainloop_signal(mainloop(), 0);
}

static void cardinfo_cb(pa_context *context, const pa_card_info *info, int isLast, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (isLast != 0 || !pulseEngine || !info) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }
    pulseEngine->cardInfoCallback(info);
}

static void sinkinfo_cb(pa_context *context, const pa_sink_info *info, int isLast, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (isLast != 0 || !pulseEngine || !info) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }
    pulseEngine->sinkInfoCallback(info);
}

static void sourceinfo_cb(pa_context *context, const pa_source_info *info, int isLast, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (isLast != 0 || !pulseEngine || !info) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }
    pulseEngine->sourceInfoCallback(info);
}

static void serverinfo_cb(pa_context *context, const pa_server_info *info, void *userdata)
{
    QPulseAudioEngineWorker *pulseEngine = static_cast<QPulseAudioEngineWorker*>(userdata);
    if (!pulseEngine || !info) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }
    pulseEngine->serverInfoCallback(info);
}

bool QPulseAudioEngineWorker::handleOperation(pa_operation *operation, const char *func_name)
{
    if (!operation) {
        qCritical("'%s' failed (lost PulseAudio connection?)", func_name);
        /* Free resources so it can retry a new connection during next operation */
        pa_threaded_mainloop_unlock(m_mainLoop);
        releasePulseContext();
        return false;
    }

    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);
    return true;
}

int QPulseAudioEngineWorker::setupVoiceCall()
{
    pa_operation *o;

    qDebug("Setting up pulseaudio for voice call");

    pa_threaded_mainloop_lock(m_mainLoop);

    /* Get and set the default sink/source to be restored later */
    o = pa_context_get_server_info(m_context, serverinfo_cb, this);
    if (!handleOperation(o, "pa_context_get_server_info"))
        return -1;

    qDebug("Recorded default sink: %s default source: %s",
            m_defaultsink.c_str(), m_defaultsource.c_str());

    /* Walk through the list of devices, find the voice call capable card and
     * identify if we have bluetooth capable devices (hsp and a2dp) */
    m_voicecallcard = m_voicecallhighest = m_voicecallprofile = "";
    m_bt_hsp = m_bt_hsp_a2dp = "";
    o = pa_context_get_card_info_list(m_context, cardinfo_cb, this);
    if (!handleOperation(o, "pa_context_get_card_info_list"))
        return -1;
    /* In case we have only one bt device that provides hsp and a2dp, we need
     * to make sure we switch the default profile for that card (to hsp) */
    if ((m_bt_hsp_a2dp != "") && (m_bt_hsp == "")) {
        qDebug("Setting PulseAudio card '%s' profile '%s'",
                m_bt_hsp_a2dp.c_str(), PULSEAUDIO_PROFILE_HSP);
        o = pa_context_set_card_profile_by_name(m_context,
            m_bt_hsp_a2dp.c_str(), PULSEAUDIO_PROFILE_HSP, success_cb, this);
        if (!handleOperation(o, "pa_context_set_card_profile_by_name"))
            return -1;
    }

    pa_threaded_mainloop_unlock(m_mainLoop);

    return 0;
}

void QPulseAudioEngineWorker::restoreVoiceCall()
{
    pa_operation *o;

    qDebug("Restoring pulseaudio previous state");

    /* Then restore previous settings */
    pa_threaded_mainloop_lock(m_mainLoop);

    /* See if we need to restore any HSP+AD2P device state */
    if ((m_bt_hsp_a2dp != "") && (m_bt_hsp == "")) {
        qDebug("Restoring PulseAudio card '%s' to profile '%s'",
                m_bt_hsp_a2dp.c_str(), PULSEAUDIO_PROFILE_A2DP);
        o = pa_context_set_card_profile_by_name(m_context,
            m_bt_hsp_a2dp.c_str(), PULSEAUDIO_PROFILE_A2DP, success_cb, this);
        if (!handleOperation(o, "pa_context_set_card_profile_by_name"))
            return;
    }

    /* Restore default sink/source */
    if (m_defaultsink != "") {
        qDebug("Restoring PulseAudio default sink to '%s'", m_defaultsink.c_str());
        o = pa_context_set_default_sink(m_context, m_defaultsink.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_default_sink"))
            return;
    }
    if (m_defaultsource != "") {
        qDebug("Restoring PulseAudio default source to '%s'", m_defaultsource.c_str());
        o = pa_context_set_default_source(m_context, m_defaultsource.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_default_source"))
            return;
    }

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void QPulseAudioEngineWorker::setCallMode(CallStatus callstatus, AudioMode audiomode)
{
    if (!createPulseContext()) {
       return;
    }
    CallStatus p_callstatus = m_callstatus;
    AudioMode p_audiomode = m_audiomode;
    AudioModes p_availableAudioModes = m_availableAudioModes;
    pa_operation *o;

    /* Check if we need to save the current pulseaudio state (e.g. when starting a call) */
    if ((callstatus != CallEnded) && (p_callstatus == CallEnded)) {
        if (setupVoiceCall() < 0) {
            qCritical("Failed to setup PulseAudio for Voice Call");
            return;
        }
    }

    /* If we have an active call, update internal state (used later when updating sink/source ports) */
    m_callstatus = callstatus;
    m_audiomode = audiomode;

    pa_threaded_mainloop_lock(m_mainLoop);

    /* Switch the virtual card mode when call is active and not active
     * This needs to be done before sink/source gets updated, because after changing mode
     * it will automatically move to input/output-parking */
    if ((m_callstatus == CallActive) && (p_callstatus != CallActive) &&
            (m_voicecallcard != "") && (m_voicecallprofile != "")) {
        qDebug("Setting PulseAudio card '%s' profile '%s'",
                m_voicecallcard.c_str(), m_voicecallprofile.c_str());
        o = pa_context_set_card_profile_by_name(m_context,
                m_voicecallcard.c_str(), m_voicecallprofile.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_card_profile_by_name"))
            return;
    } else if ((m_callstatus == CallEnded) && (m_voicecallcard != "") && (m_voicecallhighest != "")) {
        /* If using droid, make sure to restore to the profile that has the highest score */
        qDebug("Restoring PulseAudio card '%s' to profile '%s'",
                m_voicecallcard.c_str(), m_voicecallhighest.c_str());
        o = pa_context_set_card_profile_by_name(m_context,
            m_voicecallcard.c_str(), m_voicecallhighest.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_card_profile_by_name"))
            return;
    }

    /* Find highest compatible sink/source elements from the voicecall
       compatible card (on touch this means the pulse droid element) */
    m_nametoset = m_valuetoset = "";
    o = pa_context_get_sink_info_list(m_context, sinkinfo_cb, this);
    if (!handleOperation(o, "pa_context_get_sink_info_list"))
        return;
    if ((m_nametoset != "") && (m_nametoset != m_defaultsink)) {
        qDebug("Setting PulseAudio default sink to '%s'", m_nametoset.c_str());
        o = pa_context_set_default_sink(m_context, m_nametoset.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_default_sink"))
            return;
    }
    if (m_valuetoset != "") {
        qDebug("Setting PulseAudio sink '%s' port '%s'",
                m_nametoset.c_str(), m_valuetoset.c_str());
        o = pa_context_set_sink_port_by_name(m_context, m_nametoset.c_str(),
                                             m_valuetoset.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_sink_port_by_name"))
            return;
    }

    /* Same for source */
    m_nametoset = m_valuetoset = "";
    o = pa_context_get_source_info_list(m_context, sourceinfo_cb, this);
    if (!handleOperation(o, "pa_context_get_source_info_list"))
        return;
    if ((m_nametoset != "") && (m_nametoset != m_defaultsource)) {
        qDebug("Setting PulseAudio default source to '%s'", m_nametoset.c_str());
        o = pa_context_set_default_source(m_context, m_nametoset.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_default_source"))
            return;
    }
    if (m_valuetoset != "") {
        qDebug("Setting PulseAudio source '%s' port '%s'",
                m_nametoset.c_str(), m_valuetoset.c_str());
        o = pa_context_set_source_port_by_name(m_context, m_nametoset.c_str(),
                                               m_valuetoset.c_str(), success_cb, this);
        if (!handleOperation(o, "pa_context_set_source_port_by_name"))
            return;
    }

    pa_threaded_mainloop_unlock(m_mainLoop);

    /* Notify if the list of audio modes changed */
    if (p_availableAudioModes != m_availableAudioModes)
        Q_EMIT availableAudioModesChanged(m_availableAudioModes);

    /* Notify if call mode changed */
    if (p_audiomode != m_audiomode) {
        Q_EMIT audioModeChanged(m_audiomode);
    }

    /* If no more active voicecall, restore previous saved pulseaudio state */
    if (callstatus == CallEnded) {
        restoreVoiceCall();
    }

    /* In case the app had set mute when the call wasn't active, make sure we reflect it here */
    if (m_callstatus != CallEnded)
        setMicMute(m_micmute);
}

void QPulseAudioEngineWorker::setMicMute(bool muted)
{
    if (!createPulseContext()) {
       return;
    }
 
    m_micmute = muted;

    if (m_callstatus == CallEnded)
        return;

    pa_threaded_mainloop_lock(m_mainLoop);

    m_nametoset = "";
    pa_operation *o = pa_context_get_source_info_list(m_context, sourceinfo_cb, this);
    if (!handleOperation(o, "pa_context_get_source_info_list"))
        return;

    if (m_nametoset != "") {
        int m = m_micmute ? 1 : 0;
        qDebug("Setting PulseAudio source '%s' muted '%d'", m_nametoset.c_str(), m);
        o = pa_context_set_source_mute_by_name(m_context, 
            m_nametoset.c_str(), m, success_cb, this);
        if (!handleOperation(o, "pa_context_set_source_mute_by_name"))
            return;
    }

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void QPulseAudioEngineWorker::plugCardCallback(const pa_card_info *info)
{
    qDebug("Notified about card (%s) add event from PulseAudio", info->name);

    /* Check if it's indeed a BT device (with at least one hsp profile) */
    pa_card_profile_info2 *hsp = NULL, *a2dp = NULL;
    for (int i = 0; i < info->n_profiles; i++) {
        if (!strcmp(info->profiles2[i]->name, PULSEAUDIO_PROFILE_HSP))
            hsp = info->profiles2[i];
        else if (!strcmp(info->profiles2[i]->name, PULSEAUDIO_PROFILE_A2DP) && 
                  info->profiles2[i]->available != 0) {
            qDebug("Found a2dp");
            a2dp = info->profiles2[i];
        }
        qDebug("%s", info->profiles2[i]->name);
    }

    if ((!info->active_profile || !strcmp(info->active_profile->name, "off")) && a2dp) {
        qDebug("No profile set");
        m_default_bt_card_fallback = info->name;
    }

    /* We only care about BT (HSP) devices, and if one is not already available */
    if ((m_callstatus != CallEnded) && ((m_bt_hsp == "") || (m_bt_hsp_a2dp == ""))) {
        if (hsp)
            m_handleevent = true;
    }
}

void QPulseAudioEngineWorker::updateCardCallback(const pa_card_info *info)
{
    qDebug("Notified about card (%s) changes event from PulseAudio", info->name);

    /* Check if it's indeed a BT device (with at least one hsp profile) */
    pa_card_profile_info2 *hsp = NULL, *a2dp = NULL;
    for (int i = 0; i < info->n_profiles; i++) {
        if (!strcmp(info->profiles2[i]->name, PULSEAUDIO_PROFILE_HSP))
            hsp = info->profiles2[i];
        else if (!strcmp(info->profiles2[i]->name, PULSEAUDIO_PROFILE_A2DP) && 
                  info->profiles2[i]->available != 0) {
            qDebug("Found a2dp");
            a2dp = info->profiles2[i];
        }
        qDebug("%s", info->profiles2[i]->name);
    }

    if ((!info->active_profile || !strcmp(info->active_profile->name, "off")) && a2dp) {
        qDebug("No profile set");
        m_default_bt_card_fallback = info->name;
    }


    /* We only care if the card event for the voicecall capable card */
    if ((m_callstatus == CallActive) && (!strcmp(info->name, m_voicecallcard.c_str()))) {
        if (m_audiomode == AudioModeWiredHeadset) {
            /* If previous mode is wired, it means it got unplugged */
            m_handleevent = true;
            m_audiomodetoset = AudioModeBtOrWiredOrEarpiece;
        } else if ((m_audiomode == AudioModeEarpiece) || ((m_audiomode == AudioModeSpeaker))) {
            /* Now only trigger the event in case wired headset/headphone is now available */
            pa_card_port_info *port_info = NULL;
            for (int i = 0; i < info->n_ports; i++) {
                if (info->ports[i] && (info->ports[i]->available == PA_PORT_AVAILABLE_YES) && (
                            !strcmp(info->ports[i]->name, "output-wired_headset") ||
                            !strcmp(info->ports[i]->name, "output-wired_headphone"))) {
                    m_handleevent = true;
                    m_audiomodetoset = AudioModeWiredOrEarpiece;
                }
            }
        } else if (m_audiomode == AudioModeBluetooth) {
            /* Handle the event so we can update the audiomodes */
            m_handleevent = true;
            m_audiomodetoset = AudioModeBluetooth;
        }
    }
}

void QPulseAudioEngineWorker::unplugCardCallback()
{
    if (m_callstatus != CallEnded) {
        m_handleevent = true;
    }
}

void QPulseAudioEngineWorker::handleCardEvent(const int evt, const unsigned int idx)
{
    pa_operation *o = NULL;

    /* Internal state var used to know if we need to update our internal state */
    m_handleevent = false;

    if (evt == PA_SUBSCRIPTION_EVENT_NEW) {
        o = pa_context_get_card_info_by_index(m_context, idx, plug_card_cb, this);
        if (!handleOperation(o, "pa_context_get_card_info_by_index"))
            return;

        if (m_default_bt_card_fallback != "") {
            o = pa_context_set_card_profile_by_name(m_context,
                m_default_bt_card_fallback.c_str(), PULSEAUDIO_PROFILE_A2DP, success_cb, this);
            if (!handleOperation(o, "pa_context_set_card_profile_by_name"))
                return;
            m_default_bt_card_fallback = "";
        }

        if (m_handleevent) {
            qDebug("Adding new BT-HSP capable device");
            /* In case A2DP is available, switch to HSP */
            if (setupVoiceCall() < 0)
                return;
            /* Enable the HSP output port  */
            setCallMode(m_callstatus, AudioModeBluetooth);
        }
    } else if (evt == PA_SUBSCRIPTION_EVENT_CHANGE) {
        o = pa_context_get_card_info_by_index(m_context, idx, update_card_cb, this);
        if (!handleOperation(o, "pa_context_get_card_info_by_index"))
            return;

        if (m_default_bt_card_fallback != "") {
            o = pa_context_set_card_profile_by_name(m_context,
                m_default_bt_card_fallback.c_str(), PULSEAUDIO_PROFILE_A2DP, success_cb, this);
            if (!handleOperation(o, "pa_context_set_card_profile_by_name"))
                return;
            m_default_bt_card_fallback = "";
        }

        if (m_handleevent) {
            /* In this case it means the handset state changed */
            qDebug("Notifying card changes for the voicecall capable card");
            setCallMode(m_callstatus, m_audiomodetoset);
        }
    } else if (evt == PA_SUBSCRIPTION_EVENT_REMOVE) {
        /* Check if the main HSP card was removed */
        if (m_bt_hsp != "") {
            o = pa_context_get_card_info_by_name(m_context, m_bt_hsp.c_str(), unplug_card_cb, this);
            if (!handleOperation(o, "pa_context_get_sink_info_by_name"))
                return;
        }
        if (m_bt_hsp_a2dp != "") {
            o = pa_context_get_card_info_by_name(m_context, m_bt_hsp_a2dp.c_str(), unplug_card_cb, this);
            if (!handleOperation(o, "pa_context_get_sink_info_by_name"))
                return;
        }
        if (m_handleevent) {
            qDebug("Notifying about BT-HSP card removal");
            /* Needed in order to save the default sink/source */
            if (setupVoiceCall() < 0)
                return;
            /* Enable the default handset output port  */
            setCallMode(m_callstatus, AudioModeWiredOrEarpiece);
        }
    }
}

Q_GLOBAL_STATIC(QPulseAudioEngine, pulseEngine);

QPulseAudioEngine::QPulseAudioEngine(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<CallStatus>();
    qRegisterMetaType<AudioMode>();
    qRegisterMetaType<AudioModes>();
    mWorker = new QPulseAudioEngineWorker();
    QObject::connect(mWorker, SIGNAL(audioModeChanged(const AudioMode)), this, SIGNAL(audioModeChanged(const AudioMode)), Qt::QueuedConnection);
    QObject::connect(mWorker, SIGNAL(availableAudioModesChanged(const AudioModes)), this, SIGNAL(availableAudioModesChanged(const AudioModes)), Qt::QueuedConnection);
    mWorker->createPulseContext();
    mWorker->moveToThread(&mThread);
    mThread.start();
}

QPulseAudioEngine::~QPulseAudioEngine()
{
    mThread.quit();
    mThread.wait();
}

QPulseAudioEngine *QPulseAudioEngine::instance()
{
    QPulseAudioEngine *engine = pulseEngine();
    return engine;
}

void QPulseAudioEngine::setCallMode(CallStatus callstatus, AudioMode audiomode)
{
    QMetaObject::invokeMethod(mWorker, "setCallMode", Qt::QueuedConnection, Q_ARG(CallStatus, callstatus), Q_ARG(AudioMode, audiomode));
}

void QPulseAudioEngine::setMicMute(bool muted)
{
    QMetaObject::invokeMethod(mWorker, "setMicMute", Qt::QueuedConnection, Q_ARG(bool, muted));
}

QT_END_NAMESPACE

