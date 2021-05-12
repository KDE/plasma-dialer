/*
    SPDX-FileCopyrightText: 2009-2012 George Kiagiadakis <kiagiadakis.george@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "call-handler.h"
#include "call-manager.h"
#include "dialerutils.h"

#include <TelepathyQt/CallChannel>
#include <TelepathyQt/ChannelClassSpec>

static inline Tp::ChannelClassSpecList channelClassSpecList()
{
    return Tp::ChannelClassSpecList() << Tp::ChannelClassSpec::audioCall();
}

static inline Tp::AbstractClientHandler::Capabilities capabilities()
{
    Tp::AbstractClientHandler::Capabilities caps;

    // we support both audio and video in calls
    caps.setToken(TP_QT_IFACE_CHANNEL_TYPE_CALL + QLatin1String("/audio")); // clazy:exclude=qstring-allocations

    // transport methods - farstream supports them all
    caps.setToken(TP_QT_IFACE_CHANNEL_TYPE_CALL + QLatin1String("/ice")); // clazy:exclude=qstring-allocations
    caps.setToken(TP_QT_IFACE_CHANNEL_TYPE_CALL + QLatin1String("/gtalk-p2p")); // clazy:exclude=qstring-allocations
    caps.setToken(TP_QT_IFACE_CHANNEL_TYPE_CALL + QLatin1String("/shm")); // clazy:exclude=qstring-allocations

    return caps;
}

CallHandler::CallHandler(DialerUtils *utils)
    : Tp::AbstractClientHandler(channelClassSpecList(), capabilities())
{
    m_dialerUtils = utils;
    m_dialerUtils->setCallState(DialerUtils::CallState::Idle);
    qDebug() << "Call handler ready";
}

CallHandler::~CallHandler() = default;

bool CallHandler::bypassApproval() const
{
    return true;
}

void CallHandler::handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                                 const Tp::AccountPtr &account,
                                 const Tp::ConnectionPtr &connection,
                                 const QList<Tp::ChannelPtr> &channels,
                                 const QList<Tp::ChannelRequestPtr> &requestsSatisfied,
                                 const QDateTime &userActionTime,
                                 const Tp::AbstractClientHandler::HandlerInfo &handlerInfo)
{
    Q_UNUSED(account);
    Q_UNUSED(connection);
    Q_UNUSED(requestsSatisfied);
    Q_UNUSED(userActionTime);
    Q_UNUSED(handlerInfo);

    for (const Tp::ChannelPtr &channel : channels) {
        Tp::CallChannelPtr callChannel = Tp::CallChannelPtr::qObjectCast(channel);
        if (!callChannel) {
            qDebug() << "Channel is not a Call channel. Ignoring";
            continue;
        }
        // check if any call manager is already handling this channel
        if (!handledCallChannels.contains(callChannel)) {
            handledCallChannels.append(callChannel);
            new CallManager(callChannel, m_dialerUtils, this);
        }
    }

    context->setFinished();
}
