// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDBusError>
#include <QDBusServiceWatcher>
#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <qqmlregistration.h>

class LockScreenUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool lockscreenActive READ lockscreenActive NOTIFY lockscreenActiveChanged);

public:
    static LockScreenUtils *instance();
    static LockScreenUtils *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool lockscreenActive() const;
    Q_INVOKABLE void lockScreen();

Q_SIGNALS:
    void lockscreenActiveChanged();
    void lockscreenUnlocked();
    void lockscreenLocked();

public Q_SLOTS:
    void slotLockscreenActiveChanged(bool active);
    void dbusError(QDBusError error);

private:
    explicit LockScreenUtils(QObject *parent = nullptr);

    bool m_lockscreenActive = false;
    bool m_firstPropertySet = false;
};
