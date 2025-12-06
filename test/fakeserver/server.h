// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QList>
#include <QObject>

#include "callmocker.h"
#include "modemmocker.h"
#include "objectmanagermocker.h"
#include "rootmocker.h"
#include "simmocker.h"

class Server : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ModemMocker *modemMocker MEMBER m_modemMocker CONSTANT)

public:
    Server(QObject *parent = nullptr);
    void init();

    static Server *instance();

public Q_SLOTS:
    void startIncomingCall();
    void stopIncomingCall(const QString &callPath);

private:
    ObjectManagerMocker *m_objectManagerMocker{nullptr};
    ModemMocker *m_modemMocker{nullptr};
    RootMocker *m_rootMocker{nullptr};
    SimMocker *m_simMocker{nullptr};
};
