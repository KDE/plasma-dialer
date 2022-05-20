// SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <KNotification>
#ifdef HAVE_QT5_FEEDBACK
#include <QtFeedback/QFeedbackEffect>
#endif // HAVE_QT5_FEEDBACK

#include "callutilsinterface.h"
#include "contact-utils.h"

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    NotificationManager(QObject *parent = nullptr);

    void setCallUtils(org::kde::telephony::CallUtils *callUtils);
    void setContactUtils(ContactUtils *contactUtils);

private Q_SLOTS:
    void onNotificationAction(unsigned int action);
    void onCallAdded(const QString &deviceUni,
                     const QString &callUni,
                     const DialerTypes::CallDirection &callDirection,
                     const DialerTypes::CallState &callState,
                     const DialerTypes::CallStateReason &callStateReason,
                     const QString communicationWith);
    void onCallDeleted(const QString &deviceUni, const QString &callUni);
    void onCallStateChanged(const QString &deviceUni,
                            const QString &callUni,
                            const DialerTypes::CallDirection &callDirection,
                            const DialerTypes::CallState &callState,
                            const DialerTypes::CallStateReason &callStateReason);

private:
    KNotification *_ringingNotification = nullptr;

    void openRingingNotification(const QString &deviceUni, const QString &callUni, const QString communicationWith);
    void closeRingingNotification();

    void accept(const QString &deviceUni, const QString &callUni);
    void hangUp(const QString &deviceUni, const QString &callUni);

    void handleIncomingCall(const QString &deviceUni, const QString &callUni, const QString communicationWith);
    void handleRejectedCall();

    org::kde::telephony::CallUtils *_callUtils;
    ContactUtils *_contactUtils;

#ifdef HAVE_QT5_FEEDBACK
    std::unique_ptr<QFeedbackHapticsEffect> _ringEffect;
#endif // HAVE_QT5_FEEDBACK
};
