// SPDX-FileCopyrightText: 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef CALL_MANAGER_H
#define CALL_MANAGER_H

#include <QObject>
#include <TelepathyQt/CallChannel>

class DialerUtils;

class CallManager : public QObject
{
    Q_OBJECT
public:
    explicit CallManager(const Tp::CallChannelPtr &callChannel, DialerUtils *dialerUtils, QObject *parent = nullptr);
    ~CallManager() override;

private Q_SLOTS:
    void onCallStateChanged(Tp::CallState state);
    void onCallAccepted();
    void onCallRejected();
    void onHangUpRequested();
    void onSendDtmfRequested(const QString &tones);

private:
    struct Private;
    Private *const d;
};

#endif // CALL_MANAGER_H
