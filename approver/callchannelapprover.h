/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2020  <copyright holder> <email>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef CALLCHANNELAPPROVER_H
#define CALLCHANNELAPPROVER_H

#include "channelapprover.h"

#include <TelepathyQt/CallChannel>

#include <KNotification>

class CallChannelApprover : public ChannelApprover
{
    Q_OBJECT
public:
    CallChannelApprover(const Tp::CallChannelPtr& channel, QObject *parent);
    ~CallChannelApprover();

private slots:
    void onChannelReady(Tp::PendingOperation *op);
    void onNotificationAction(unsigned int action);
    
private:
    QMap<Tp::PendingOperation*,Tp::ChannelPtr> m_Channels;
    KNotification *m_ringingNotification = nullptr;
};

#endif // CALLCHANNELAPPROVER_H
