// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef CALL_MANAGER_H
#define CALL_MANAGER_H

#include <QObject>
#include <kTelephonyMetaTypes/dialer-types.h>

#include "callutilsinterface.h"

#include "contact-utils.h"
#include "dialer-utils.h"

class DialerManager : public QObject
{
    Q_OBJECT
public:
    explicit DialerManager(QObject *parent = nullptr);
    ~DialerManager() override;

    void setCallUtils(org::kde::telephony::CallUtils *callUtils);
    void setContactUtils(ContactUtils *contactUtils);
    void setDialerUtils(DialerUtils *dialerUtils);

private Q_SLOTS:
    void onCallStateChanged(const QString &deviceUni,
                            const QString &callUni,
                            const DialerTypes::CallDirection &callDirection,
                            const DialerTypes::CallState &callState,
                            const DialerTypes::CallStateReason &callStateReason);
    void onSetSpeakerModeRequested(bool enabled);
    void onSetMuteRequested(bool muted);

private:
    ContactUtils *_contactUtils;
    DialerUtils *_dialerUtils;
};

#endif // CALL_MANAGER_H
