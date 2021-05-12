// SPDX-FileCopyrightText: 2010 Collabora Ltd. <info@collabora.co.uk>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "channelapprover.h"

#include "callchannelapprover.h"

ChannelApprover *ChannelApprover::create(const Tp::ChannelPtr &channel, QObject *parent)
{
    if (channel->channelType() == TP_QT_IFACE_CHANNEL_TYPE_CALL) {
        return new CallChannelApprover(Tp::CallChannelPtr::dynamicCast(channel), parent);
    }
    return nullptr;
}
