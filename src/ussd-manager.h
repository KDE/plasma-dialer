// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#ifndef USSD_MANAGER_H
#define USSD_MANAGER_H

#include <QObject>
#include <TelepathyQt/Connection>

class DialerUtils;

class UssdManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)

public:
    explicit UssdManager(const Tp::ConnectionPtr &connection, DialerUtils *dialerUtils, QObject *parent = nullptr);
    ~UssdManager() override;

    bool active() const;
    QString state() const;

private Q_SLOTS:
    void onInitiated(const QString &command);
    void onInitiateComplete(const QString &command);
    void onResponded(const QString &reply);
    void onCanceled();

    void onStateChanged(const QString &state);

Q_SIGNALS:
    void activeChanged();
    void stateChanged(const QString &state);

private:
    struct Private;
    Private *const d;
};

#endif // USSD_MANAGER_H
