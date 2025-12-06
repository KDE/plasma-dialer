// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include <QDBusObjectPath>
#include <QList>
#include <QVariantMap>

#include <ModemManagerQt/Manager>

#include "callmocker.h"
#include "dbusobject.h"
#include "objectmanagermocker.h"
#include "simmocker.h"
#include "types.h"

// Mock for org.freedesktop.ModemManager1.Modem.xml
class ModemMocker : public QObject, public DBusObject
{
    Q_OBJECT
    // Modem
    Q_PROPERTY(QDBusObjectPath Sim READ sim NOTIFY simChanged)
    Q_PROPERTY(QList<QDBusObjectPath> Bearers READ bearers NOTIFY bearersChanged)
    // 3gpp
    Q_PROPERTY(QString Imei READ imei NOTIFY imeiChanged)
    Q_PROPERTY(int RegistrationState READ registrationState NOTIFY registrationStateChanged)
    Q_PROPERTY(QString OperatorCode READ operatorCode NOTIFY operatorCodeChanged)
    Q_PROPERTY(QString OperatorName READ operatorName NOTIFY operatorNameChanged)
    // 3gpp USSD
    Q_PROPERTY(int State READ state NOTIFY stateChanged)
    Q_PROPERTY(QString NetworkNotification READ networkNotification NOTIFY networkNotificationChanged)
    Q_PROPERTY(QString NetworkRequest READ networkRequest NOTIFY networkRequestChanged)
    // Voice
    Q_PROPERTY(QStringList Calls READ calls NOTIFY callsChanged)

    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.ModemManager1.Modem")

public:
    ModemMocker(SimMocker *sim = nullptr, ObjectManagerMocker *objectManager = nullptr, QObject *parent = nullptr);

    // Modem
    QDBusObjectPath sim() const;
    QList<QDBusObjectPath> bearers() const;

    // 3gpp
    QString imei() const;
    int registrationState() const;
    QString operatorCode() const;
    QString operatorName() const;

    // 3gpp USSD
    int state() const;
    QString networkNotification() const;
    QString networkRequest() const;

    // Voice
    QStringList calls() const;

    // Other
    QDBusObjectPath addCall(QVariantMap props);
    void removeCall(QDBusObjectPath callPath);
    void registerObject();

public Q_SLOTS:
    // 3gpp
    Q_SCRIPTABLE void Register(QString operatorId);
    Q_SCRIPTABLE QList<QVariantMap> Scan();

    // 3gpp USSD
    Q_SCRIPTABLE QString Initiate(const QString &command);
    Q_SCRIPTABLE QString Respond(const QString &response);
    Q_SCRIPTABLE void Cancel();

    // Voice
    Q_SCRIPTABLE QList<QDBusObjectPath> ListCalls();
    Q_SCRIPTABLE void DeleteCall(QDBusObjectPath path);
    Q_SCRIPTABLE QDBusObjectPath CreateCall(QVariantMap properties);

Q_SIGNALS:
    // Modem
    void simChanged();
    void bearersChanged();

    // 3gpp
    void imeiChanged();
    void registrationStateChanged();
    void operatorCodeChanged();
    void operatorNameChanged();

    // 3gpp USSD
    void stateChanged();
    void networkNotificationChanged();
    void networkRequestChanged();

    // Voice
    void CallAdded(QDBusObjectPath path);
    void CallDeleted(QDBusObjectPath path);
    void callsChanged();

private:
    QList<CallMocker *> m_calls;

    QDBusObjectPath m_simPath;
    bool m_initialized{false};

    ObjectManagerMocker *m_objectManagerMocker{nullptr};

    int m_callNum = 0;
};
