// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include <QDBusObjectPath>
#include <QList>
#include <QVariantMap>

#include <ModemManager/ModemManager.h>
#include <ModemManagerQt/Manager>

#include "dbusobject.h"
#include "types.h"

// Mock for org.freedesktop.ModemManager1.Sim.xml
class SimMocker : public QObject, public DBusObject
{
    Q_OBJECT
    Q_PROPERTY(bool Active READ active NOTIFY activeChanged)
    Q_PROPERTY(QString SimIdentifier READ simIdentifier NOTIFY simIdentifierChanged)
    Q_PROPERTY(QString Imsi READ imsi NOTIFY imsiChanged)
    Q_PROPERTY(QString Eid READ eid NOTIFY eidChanged)
    Q_PROPERTY(QString OperatorIdentifier READ operatorIdentifier NOTIFY operatorIdentifierChanged)
    Q_PROPERTY(QString OperatorName READ operatorName NOTIFY operatorNameChanged)
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.ModemManager1.Sim")

public:
    SimMocker(QObject *parent = nullptr);

    bool active() const
    {
        return true;
    }
    QString simIdentifier() const
    {
        return QStringLiteral("89014103211118510720");
    }
    QString imsi() const
    {
        return QStringLiteral("310260000000000");
    }
    QString eid() const
    {
        return QStringLiteral("89049032000000000000000000000000");
    }
    QString operatorIdentifier() const
    {
        return QStringLiteral("310260");
    }
    QString operatorName() const
    {
        return QStringLiteral("Test Operator");
    }

    void registerObject();

Q_SIGNALS:
    void activeChanged();
    void simIdentifierChanged();
    void imsiChanged();
    void eidChanged();
    void operatorIdentifierChanged();
    void operatorNameChanged();

private:
    bool m_initialized{false};
};
