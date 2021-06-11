// SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#ifndef CALLCHANNELAPPROVER_H
#define CALLCHANNELAPPROVER_H

#include "channelapprover.h"

#include <TelepathyQt/CallChannel>

#include <KNotification>

class CallChannelApprover : public ChannelApprover
{
    Q_OBJECT
public:
    CallChannelApprover(const Tp::CallChannelPtr &channel, QObject *parent);
    ~CallChannelApprover();

private slots:
    void onChannelReady(Tp::PendingOperation *op);
    void onNotificationAction(unsigned int action);

private:
    void pauseMedia();
    void unpauseMedia();

    QMap<Tp::PendingOperation *, Tp::ChannelPtr> m_Channels;
    KNotification *m_ringingNotification = nullptr;
    QSet<QString> m_pausedSources;
};

#endif // CALLCHANNELAPPROVER_H
