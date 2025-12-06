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

// Mock for org.freedesktop.ModemManager1.Call.xml
class CallMocker : public QObject, public DBusObject
{
    Q_OBJECT
    Q_PROPERTY(int State READ state NOTIFY stateChanged2)
    Q_PROPERTY(int StateReason READ stateReason NOTIFY stateReasonChanged)
    Q_PROPERTY(int Direction READ direction NOTIFY directionChanged)
    Q_PROPERTY(QString Number READ number NOTIFY numberChanged)
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.ModemManager1.Call")

public:
    CallMocker(int num = 0, QVariantMap props = {}, QObject *parent = nullptr);

    int state() const;
    int stateReason() const;
    int direction() const;
    QString number() const;

    void registerObject();

public Q_SLOTS:
    Q_SCRIPTABLE void Start();
    Q_SCRIPTABLE void Accept();
    Q_SCRIPTABLE void Hangup();
    Q_SCRIPTABLE void SendDtmf(const QString &dtmf);

Q_SIGNALS:
    void DtmfReceived(const QString &dtmf);
    void StateChanged(int oldState, int newState, uint reason);
    void stateChanged2();
    void stateReasonChanged();
    void directionChanged();
    void numberChanged();

    // Custom
    void hangupRequested();

private:
    void setState(int state, int reason);

    bool m_initialized{false};
    QVariantMap m_props;

    int m_callState;
    int m_callStateReason;
    int m_callDirection;

    QString m_number;
};
