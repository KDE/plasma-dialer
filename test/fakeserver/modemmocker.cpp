// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "modemmocker.h"
#include "modem3gppadaptor.h"
#include "modem3gppussdadaptor.h"
#include "modemadaptor.h"
#include "modemvoiceadaptor.h"

#include <ModemManager/ModemManager.h>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QTimer>

using namespace Qt::StringLiterals;

ModemMocker::ModemMocker(SimMocker *sim, ObjectManagerMocker *objectManager, QObject *parent)
    : QObject{parent}
    , m_simPath{sim ? sim->dbusPath() : QDBusObjectPath{}}
    , m_objectManagerMocker{objectManager}
{
    setDBusName(QStringLiteral("org.freedesktop.ModemManager1.Modem"));
    setDBusPath(QDBusObjectPath(QStringLiteral("/org/freedesktop/ModemManager1/Modem/0")));

    m_ussdStateTimer.setSingleShot(true);
}

QDBusObjectPath ModemMocker::sim() const
{
    return m_simPath;
}
QList<QDBusObjectPath> ModemMocker::bearers() const
{
    return {};
}

QString ModemMocker::imei() const
{
    return {};
}

int ModemMocker::registrationState() const
{
    return 0;
}

QString ModemMocker::operatorCode() const
{
    return QStringLiteral("310260");
}

QString ModemMocker::operatorName() const
{
    return QStringLiteral("Test Operator");
}

int ModemMocker::state() const
{
    return m_ussdState;
}

QString ModemMocker::networkNotification() const
{
    return m_networkNotification;
}

QString ModemMocker::networkRequest() const
{
    return m_networkRequest;
}

void ModemMocker::setUssdState(int state)
{
    if (m_ussdState == state) {
        return;
    }
    m_ussdState = state;
    emitUssdPropertyChanged(u"State"_s, QVariant::fromValue(static_cast<uint>(state)));
    Q_EMIT stateChanged();
}

void ModemMocker::setNetworkNotification(const QString &message)
{
    m_networkNotification = message;
    emitUssdPropertyChanged(u"NetworkNotification"_s, QVariant::fromValue(message));
    Q_EMIT networkNotificationChanged();
}

void ModemMocker::setNetworkRequest(const QString &message)
{
    m_networkRequest = message;
    emitUssdPropertyChanged(u"NetworkRequest"_s, QVariant::fromValue(message));
    Q_EMIT networkRequestChanged();
}

void ModemMocker::emitUssdPropertyChanged(const QString &name, const QVariant &value)
{
    QVariantMap updatedProperties;
    updatedProperties[name] = value;

    QDBusMessage signal = QDBusMessage::createSignal(dbusPath().path(), u"org.freedesktop.DBus.Properties"_s, u"PropertiesChanged"_s);
    signal << u"org.freedesktop.ModemManager1.Modem.Modem3gpp.Ussd"_s;
    signal << updatedProperties;
    signal << QStringList();
    QDBusConnection::systemBus().send(signal);
}

QStringList ModemMocker::calls() const
{
    qDebug() << "ModemVoiceMocker calls()";
    QStringList list;
    for (CallMocker *call : m_calls) {
        list.append(call->dbusPath().path());
    }
    return list;
}

void ModemMocker::Register(QString operatorId)
{
    qDebug() << "3gpp Register:" << operatorId;
}

QList<QVariantMap> ModemMocker::Scan()
{
    qDebug() << "3gpp Scan";
    return {};
}

// 3gpp USSD
QString ModemMocker::Initiate(const QString &command)
{
    qDebug() << "3gppUssd Initiate:" << command;

    if (m_ussdState != MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE) {
        qWarning() << "3gppUssd Initiate: session already active, ignoring";
        return {};
    }

    setUssdState(MM_MODEM_3GPP_USSD_SESSION_STATE_ACTIVE);

    QString response;
    if (command == u"*123#"_s || command == u"*100#"_s) {
        response = u"Your balance is $42.00. Reply 1 for details, 2 for top-up."_s;
    } else {
        response = u"USSD response for: "_s + command;
    }

    // Transition to USER_RESPONSE after returning (multi-step session)
    m_ussdStateTimer.disconnect();
    connect(&m_ussdStateTimer, &QTimer::timeout, this, [this]() {
        setUssdState(MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE);
    });
    m_ussdStateTimer.start(100);

    return response;
}

QString ModemMocker::Respond(const QString &response)
{
    qDebug() << "3gppUssd Respond:" << response;

    if (m_ussdState != MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE) {
        qWarning() << "3gppUssd Respond: no active session expecting response";
        return {};
    }

    QString reply = u"Response received: "_s + response;

    // End session after responding
    m_ussdStateTimer.disconnect();
    connect(&m_ussdStateTimer, &QTimer::timeout, this, [this]() {
        setUssdState(MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE);
    });
    m_ussdStateTimer.start(100);

    return reply;
}

void ModemMocker::Cancel()
{
    qDebug() << "3gppUssd Cancel";
    m_ussdStateTimer.stop();
    setNetworkNotification({});
    setNetworkRequest({});
    setUssdState(MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE);
}

// Voice
QList<QDBusObjectPath> ModemMocker::ListCalls()
{
    qDebug() << "ModemVoiceMocker ListCalls";
    QList<QDBusObjectPath> list;
    for (CallMocker *call : m_calls) {
        list.append(call->dbusPath());
    }
    return list;
}

void ModemMocker::DeleteCall(QDBusObjectPath path)
{
    qDebug() << "ModemVoiceMocker DeleteCall:" << path;

    removeCall(path);
}

QDBusObjectPath ModemMocker::CreateCall(QVariantMap properties)
{
    qDebug() << "ModemVoiceMocker CreateCall:" << properties;

    QVariantMap props;
    props[u"number"_s] = properties.value(u"number"_s).toString();
    props[u"direction"_s] = u"outgoing"_s;
    return addCall(props);
}

QDBusObjectPath ModemMocker::addCall(QVariantMap props)
{
    qDebug() << "Add call with props:" << props;
    auto *call = new CallMocker(m_callNum++, props, this);
    call->registerObject();
    m_objectManagerMocker->addObject(call);

    connect(call, &CallMocker::hangupRequested, this, [this, call]() {
        removeCall(call->dbusPath());
    });

    m_calls.append(call);

    Q_EMIT callsChanged();
    Q_EMIT CallAdded(call->dbusPath());
    return call->dbusPath();
}

void ModemMocker::removeCall(QDBusObjectPath callPath)
{
    auto it = std::find_if(m_calls.begin(), m_calls.end(), [callPath](CallMocker *call) {
        return call->dbusPath() == callPath;
    });

    if (it == m_calls.end()) {
        return;
    }

    (*it)->deleteLater();
    m_calls.erase(it);
    Q_EMIT callsChanged();
    Q_EMIT CallDeleted(callPath);
}

void ModemMocker::registerObject()
{
    if (!m_initialized) {
        qDebug() << "Initializing dbus mock for org.freedesktop.ModemManager1.Modem...";
        qDebug() << "Initializing dbus mock for org.freedesktop.ModemManager1.Modem.Modem3gpp...";
        qDebug() << "Initializing dbus mock for org.freedesktop.ModemManager1.Modem.Modem3gpp.Ussd...";
        qDebug() << "Initializing dbus mock for org.freedesktop.ModemManager1.Modem.Voice...";

        new ModemAdaptor{this};
        new Modem3gppAdaptor{this};
        new UssdAdaptor{this};
        new VoiceAdaptor{this};

        auto bus = QDBusConnection::systemBus();
        bool status = bus.registerObject(dbusPath().path(), this, QDBusConnection::ExportAdaptors);

        if (status) {
            m_initialized = true;
        } else {
            qDebug() << "Failed to init!";
        }
    }
}
