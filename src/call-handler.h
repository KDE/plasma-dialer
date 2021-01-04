/*
    SPDX-FileCopyrightText: 2009 George Kiagiadakis <kiagiadakis.george@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef CALL_HANDLER_H
#define CALL_HANDLER_H

#include <TelepathyQt/AbstractClientHandler>

class DialerUtils;

class CallHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    CallHandler(DialerUtils *utils);
    ~CallHandler() override;

    bool bypassApproval() const override;
    void handleChannels(const Tp::MethodInvocationContextPtr<> & context,
                                const Tp::AccountPtr & account,
                                const Tp::ConnectionPtr & connection,
                                const QList<Tp::ChannelPtr> & channels,
                                const QList<Tp::ChannelRequestPtr> & requestsSatisfied,
                                const QDateTime & userActionTime,
                                const Tp::AbstractClientHandler::HandlerInfo & handlerInfo) override;
private:
    QList<Tp::CallChannelPtr> handledCallChannels;
    DialerUtils *m_dialerUtils;
};

#endif
