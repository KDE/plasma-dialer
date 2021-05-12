/*
    SPDX-FileCopyrightText: 2010 Collabora Ltd. <info@collabora.co.uk>
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#ifndef DISPATCHOPERATION_H
#define DISPATCHOPERATION_H

#include <QtCore/QObject>
#include <TelepathyQt/ChannelDispatchOperation>

class ChannelApprover;

class DispatchOperation : public QObject
{
    Q_OBJECT
public:
    DispatchOperation(const Tp::ChannelDispatchOperationPtr &dispatchOperation, QObject *parent);
    ~DispatchOperation() override;

private Q_SLOTS:
    void onChannelLost(const Tp::ChannelPtr &channel, const QString &errorName, const QString &errorMessage);
    void onDispatchOperationInvalidated(Tp::DBusProxy *proxy, const QString &errorName, const QString &errorMessage);
    void onChannelAccepted();
    void onChannelRejected();
    void onClaimFinished(Tp::PendingOperation *operation);

private:
    Tp::ChannelDispatchOperationPtr m_dispatchOperation;
    QHash<Tp::ChannelPtr, ChannelApprover *> m_channelApprovers;
};

#endif // DISPATCHOPERATION_H
