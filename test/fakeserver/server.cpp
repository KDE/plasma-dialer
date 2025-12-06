// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "server.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDateTime>

using namespace Qt::StringLiterals;

Server::Server(QObject *parent)
    : QObject{parent}
{
    init();
}

Server *Server::instance()
{
    static Server *server = new Server(nullptr);
    return server;
}

void Server::init()
{
    auto bus = QDBusConnection::systemBus();
    bool status = bus.registerService(QStringLiteral("org.freedesktop.ModemManager1"));

    if (!status) {
        qDebug() << "Failed to register service! Is ModemManager already running? You need to stop it first.";
        QCoreApplication::quit();
        return;
    }

    m_objectManagerMocker = new ObjectManagerMocker(this);
    m_objectManagerMocker->registerObject();

    m_simMocker = new SimMocker(this);
    m_simMocker->registerObject();
    m_objectManagerMocker->addObject(m_simMocker);

    m_modemMocker = new ModemMocker(m_simMocker, m_objectManagerMocker, this);
    m_modemMocker->registerObject();
    m_objectManagerMocker->addObject(m_modemMocker);

    m_rootMocker = new RootMocker(this);
    m_rootMocker->registerObject();
    m_objectManagerMocker->addObject(m_rootMocker);
}

void Server::startIncomingCall()
{
    QVariantMap props;
    props[u"number"_s] = QStringLiteral("1234567890");
    props[u"direction"_s] = QStringLiteral("incoming");
    m_modemMocker->addCall(props);
}

void Server::stopIncomingCall(const QString &callPath)
{
    m_modemMocker->removeCall(QDBusObjectPath{callPath});
}
