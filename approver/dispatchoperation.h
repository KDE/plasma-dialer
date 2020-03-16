/*
    Copyright (C) 2010 Collabora Ltd. <info@collabora.co.uk>
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    DispatchOperation(const Tp::ChannelDispatchOperationPtr & dispatchOperation, QObject *parent);
    ~DispatchOperation() override;


private Q_SLOTS:
    void onChannelLost(const Tp::ChannelPtr & channel,
                       const QString & errorName,
                       const QString & errorMessage);
    void onDispatchOperationInvalidated(Tp::DBusProxy *proxy,
                                        const QString & errorName,
                                        const QString & errorMessage);
    void onChannelAccepted();
    void onChannelRejected();
    void onClaimFinished(Tp::PendingOperation *operation);

private:
    Tp::ChannelDispatchOperationPtr m_dispatchOperation;
    QHash<Tp::ChannelPtr, ChannelApprover*> m_channelApprovers;
};

#endif // DISPATCHOPERATION_H
