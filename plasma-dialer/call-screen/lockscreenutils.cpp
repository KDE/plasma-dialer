// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "lockscreenutils.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QTimer>

LockScreenUtils *LockScreenUtils::instance()
{
    static auto singleton = new LockScreenUtils;
    return singleton;
}

LockScreenUtils *LockScreenUtils::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

LockScreenUtils::LockScreenUtils(QObject *parent)
    : QObject{parent}
{
    QDBusMessage request = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("/ScreenSaver"),
                                                          QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("GetActive"));

    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(request);
    if (reply.isValid()) {
        m_lockscreenActive = reply.value();
    } else {
        qDebug() << "Error fetching lockscreen state using DBus:" << reply.error().message();
    }

    QDBusConnection::sessionBus().connect(QStringLiteral("org.freedesktop.ScreenSaver"),
                                          QStringLiteral("/ScreenSaver"),
                                          QStringLiteral("org.freedesktop.ScreenSaver"),
                                          QStringLiteral("ActiveChanged"),
                                          this,
                                          SLOT(slotLockscreenActiveChanged(bool)));
}

bool LockScreenUtils::lockscreenActive() const
{
    return m_lockscreenActive;
}

void LockScreenUtils::lockScreen()
{
    QDBusMessage request = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("/ScreenSaver"),
                                                          QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("Lock"));
    QDBusConnection::sessionBus().asyncCall(request);
}

void LockScreenUtils::slotLockscreenActiveChanged(bool active)
{
    if (active != m_lockscreenActive) {
        m_lockscreenActive = active;

        Q_EMIT lockscreenActiveChanged();
        m_lockscreenActive ? Q_EMIT lockscreenLocked() : Q_EMIT lockscreenUnlocked();
    }
}

void LockScreenUtils::dbusError(QDBusError error)
{
    qDebug() << "Error fetching lockscreen state using DBus:" << error.message();
}

#include "moc_lockscreenutils.cpp"
