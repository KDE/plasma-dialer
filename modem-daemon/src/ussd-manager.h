// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

class ModemController;
class UssdUtils;

class UssdManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)

public:
    explicit UssdManager(ModemController *modemController, UssdUtils *ussdUtils, QObject *parent = nullptr);

    bool active() const;
    QString state() const;

private Q_SLOTS:
    void onInitiated(const QString &deviceUni, const QString &command);
    void onInitiateComplete(const QString &deviceUni, const QString &command);
    void onResponded(const QString &deviceUni, const QString &reply);
    void onCanceled(const QString &deviceUni);

    void onStateChanged(const QString &deviceUni, const QString &state);

Q_SIGNALS:
    void activeChanged(const QString &deviceUni);
    void stateChanged(const QString &deviceUni, const QString &state);

private:
    UssdUtils *_ussdUtils;
    ModemController *_modemController;
    QString _state;
};
