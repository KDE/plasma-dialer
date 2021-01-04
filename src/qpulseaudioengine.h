/****************************************************************************
**
** SPDX-FileCopyrightText: 2013 Digia Plc and /or its subsidiary(-ies).
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

#ifndef QPULSEAUDIOENGINE_H
#define QPULSEAUDIOENGINE_H

#include <QtCore/qmap.h>
#include <QtCore/qbytearray.h>
#include <QThread>
#include <pulse/pulseaudio.h>

enum AudioMode {
    AudioModeEarpiece = 0x0001,
    AudioModeWiredHeadset = 0x0002,
    AudioModeSpeaker = 0x0004,
    AudioModeBluetooth = 0x0008,
    AudioModeBtOrWiredOrEarpiece = AudioModeBluetooth | AudioModeWiredHeadset | AudioModeEarpiece,
    AudioModeWiredOrEarpiece = AudioModeWiredHeadset | AudioModeEarpiece,
    AudioModeWiredOrSpeaker = AudioModeWiredHeadset | AudioModeSpeaker,
    AudioModeBtOrWiredOrSpeaker = AudioModeBluetooth | AudioModeWiredOrSpeaker
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

QT_BEGIN_NAMESPACE

class QPulseAudioEngineWorker : public QObject
{
    Q_OBJECT

public:
    QPulseAudioEngineWorker(QObject *parent = nullptr);
    ~QPulseAudioEngineWorker();

    pa_threaded_mainloop *mainloop() { return m_mainLoop; }
    pa_context *context() { return m_context; }
    bool createPulseContext(void);
    int setupVoiceCall(void);
    void restoreVoiceCall(void);
    /* Callbacks to be used internally */
    void cardInfoCallback(const pa_card_info *card);
    void sinkInfoCallback(const pa_sink_info *sink);
    void sourceInfoCallback(const pa_source_info *source);
    void serverInfoCallback(const pa_server_info *server);
    void plugCardCallback(const pa_card_info *card);
    void updateCardCallback(const pa_card_info *card);
    void unplugCardCallback();

Q_SIGNALS:
    void audioModeChanged(const AudioMode mode);
    void availableAudioModesChanged(const AudioModes modes);

public Q_SLOTS:
    void handleCardEvent(const int evt, const unsigned int idx);
    void setCallMode(CallStatus callstatus, AudioMode audiomode);
    void setMicMute(bool muted); /* True if muted, false if unmuted */

private:
    pa_mainloop_api *m_mainLoopApi;
    pa_threaded_mainloop *m_mainLoop;
    pa_context *m_context;

    AudioModes m_availableAudioModes;
    CallStatus m_callstatus;
    AudioMode m_audiomode;
    AudioMode m_audiomodetoset;
    bool m_micmute;
    bool m_handleevent;
    std::string m_nametoset;
    std::string m_valuetoset;
    std::string m_defaultsink;
    std::string m_defaultsource;
    std::string m_voicecallhighest;
    std::string m_bt_hsp;
    std::string m_bt_hsp_a2dp;
    std::string m_default_bt_card_fallback;
    std::string m_voicecallcard;
    std::string m_voicecallprofile;

    bool handleOperation(pa_operation *operation, const char *func_name);
    void releasePulseContext(void);
};

class QPulseAudioEngine : public QObject
{
    Q_OBJECT

public:
    explicit QPulseAudioEngine(QObject *parent = nullptr);
    ~QPulseAudioEngine();
    static QPulseAudioEngine *instance();

    void setCallMode(CallStatus callstatus, AudioMode audiomode);
    void setMicMute(bool muted); /* True if muted, false if unmuted */

Q_SIGNALS:
    void audioModeChanged(const AudioMode mode);
    void availableAudioModesChanged(const AudioModes modes);
private:
    QPulseAudioEngineWorker *mWorker;
    QThread mThread;
};

QT_END_NAMESPACE

#endif
