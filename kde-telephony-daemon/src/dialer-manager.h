// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef CALL_MANAGER_H
#define CALL_MANAGER_H

#include <QObject>
#include <kTelephonyMetaTypes/dialer-types.h>

#include "callutilsinterface.h"

#include "dialer-utils.h"

class DialerManager : public QObject
{
    Q_OBJECT
public:
    explicit DialerManager(QObject *parent = nullptr);
    ~DialerManager() override;

    void setCallUtils(org::kde::telephony::CallUtils *callUtils);
    void setDialerUtils(DialerUtils *dialerUtils);

private Q_SLOTS:
    void onCallAdded(const QString &deviceUni,
                     const QString &callUni,
                     const DialerTypes::CallDirection &callDirection,
                     const DialerTypes::CallState &callState,
                     const DialerTypes::CallStateReason &callStateReason,
                     const QString communicationWith);
    void onFetchedCallsChanged(const DialerTypes::CallDataVector &fetchedCalls);
    void onCallStateChanged(const DialerTypes::CallData &callData);
    void onUtilsMuteChanged(bool muted);
    void onUtilsSpeakerModeChanged(bool enabled);
    void onUtilsMuteRequested();
    void onUtilsSpeakerModeRequested();

private:
    void pauseMedia();
    void unpauseMedia();

    org::kde::telephony::CallUtils *_callUtils;
    DialerUtils *_dialerUtils;
    QSet<QString> _pausedSources;

    bool _needsDefaultAudioMode;
};

#endif // CALL_MANAGER_H
