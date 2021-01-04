/*
    SPDX-FileCopyrightText: 2010 Collabora Ltd. <info@collabora.co.uk>
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#ifndef CHANNELAPPROVER_H
#define CHANNELAPPROVER_H

#include <QtCore/QObject>
#include <TelepathyQt/Channel>

class ChannelApprover : public QObject
{
    Q_OBJECT
public:
    static ChannelApprover *create(const Tp::ChannelPtr & channel, QObject *parent);

Q_SIGNALS:
    void channelAccepted();
    void channelRejected();

protected:
    inline ChannelApprover(QObject *parent) : QObject(parent) {}
};

#endif // CHANNELAPPROVER_H
