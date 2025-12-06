// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "simmocker.h"
#include "simadaptor.h"

#include <QDBusConnection>

SimMocker::SimMocker(QObject *parent)
    : QObject{parent}
{
    setDBusName(QStringLiteral("org.freedesktop.ModemManager1.Sim"));
    setDBusPath(QDBusObjectPath(QStringLiteral("/org/freedesktop/ModemManager1/Sim/0")));
}

void SimMocker::registerObject()
{
    if (!m_initialized) {
        qDebug() << "Initializing dbus mock for org.freedesktop.ModemManager1.Sim...";

        new SimAdaptor{this};

        auto bus = QDBusConnection::systemBus();
        bool status = bus.registerObject(dbusPath().path(), this, QDBusConnection::ExportAdaptors);

        if (status) {
            m_initialized = true;
        } else {
            qDebug() << "Failed to init!";
        }
    }
}
