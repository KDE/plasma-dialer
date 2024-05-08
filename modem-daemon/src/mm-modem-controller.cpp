// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "mm-modem-controller.h"

ModemManagerController::ModemManagerController(QObject *parent)
    : ModemController(parent)
{
    init();
    connect(ModemManager::notifier(), &ModemManager::Notifier::modemAdded, this, [this](const QString &udi) {
        Q_UNUSED(udi);
        init();
    });
    connect(ModemManager::notifier(), &ModemManager::Notifier::modemRemoved, this, [this](const QString &udi) {
        Q_UNUSED(udi);
        init();
    });
}

QString ModemManagerController::subsystem()
{
    return QStringLiteral("mm");
}

QString ModemManagerController::equipmentIdentifier(const QString &deviceUni)
{
    QString equipmentIdentifier;
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return equipmentIdentifier;
    }
    const auto modemIface = modem->modemInterface();
    if (modemIface.isNull()) {
        qDebug() << Q_FUNC_INFO << "modemIface not found";
        return equipmentIdentifier;
    }
    return modemIface->equipmentIdentifier();
}

void ModemManagerController::ussdInitiate(const QString &deviceUni, const QString &command)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        Q_EMIT ussdErrorReceived(deviceUni, tr("deviceUni not found"));
        return;
    }
    const auto modem3gppUssdInterface = this->modem3gppUssdInterface(modem);
    QDBusPendingReply<QString> reply = modem3gppUssdInterface->initiate(command);
    /*
        During the following call to waitForFinished(), the DBus client waits for the DBus server to send a reply.
        In the meantime, the ModemManager instance will send the USSD request and wait for an USSD response.
        Once the ModemManager instance has received an USSD response, it will send a DBus response as well as a state update, in a possibly undefined order.
        Even if the state update would be sent before the DBus response, it will be received from this code after the DBus response, as waitForFinished() waits for the correct DBus response and queues any other DBus activity (such as the state update).
        Hence, the state update will always be received later than waitForFinished() returns.
        Hence, the ussdInitiateComplete() call will contain the correct message, but the corresponding state update will only happen (milliseconds) later.
    */
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        Q_EMIT ussdErrorReceived(deviceUni, reply.error().message());
        return;
    }
    Q_EMIT ussdInitiateComplete(deviceUni, reply.value());
}

void ModemManagerController::ussdRespond(const QString &deviceUni, const QString &reply)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        Q_EMIT ussdErrorReceived(deviceUni, tr("deviceUni not found"));
        return;
    }
    const auto modem3gppUssdInterface = this->modem3gppUssdInterface(modem);
    QDBusPendingReply<QString> dbusReply = modem3gppUssdInterface->respond(reply);
    dbusReply.waitForFinished();
    if (dbusReply.isError()) {
        qDebug() << Q_FUNC_INFO << dbusReply.error();
        Q_EMIT ussdErrorReceived(deviceUni, dbusReply.error().message());
        return;
    }
}

void ModemManagerController::ussdCancel(const QString &deviceUni)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        Q_EMIT ussdErrorReceived(deviceUni, tr("deviceUni not found"));
        return;
    }
    const auto modem3gppUssdInterface = this->modem3gppUssdInterface(modem);
    modem3gppUssdInterface->cancel();
}

QString ModemManagerController::callNumber(const QString &deviceUni, const QString &callUni)
{
    QString callNumber;
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return callNumber;
    }
    const auto voiceInterface = this->voiceInterface(modem);
    if (voiceInterface.isNull()) {
        qDebug() << Q_FUNC_INFO << "voiceInterface not found";
        return callNumber;
    }
    const auto call = voiceInterface->findCall(callUni);
    if (call.isNull()) {
        qDebug() << Q_FUNC_INFO << "call not found" << callUni;
        return callNumber;
    }
    return call->number();
}

void ModemManagerController::createCall(const QString &deviceUni, const QString &callUni)
{
    qDebug() << Q_FUNC_INFO << "creating call";
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto voiceInterface = this->voiceInterface(modem);
    if (voiceInterface.isNull()) {
        qDebug() << Q_FUNC_INFO << "voiceInterface not found";
        return;
    }
    QDBusPendingReply<QDBusObjectPath> reply = voiceInterface->createCall(callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        return;
    }
}

void ModemManagerController::acceptCall(const QString &deviceUni, const QString &callUni)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto voiceInterface = this->voiceInterface(modem);
    if (voiceInterface.isNull()) {
        qDebug() << Q_FUNC_INFO << "voiceInterface not found";
        return;
    }
    const auto call = voiceInterface->findCall(callUni);
    if (call.isNull()) {
        qDebug() << Q_FUNC_INFO << "call not found" << callUni;
        return;
    }
    call->accept();
}

void ModemManagerController::hangUp(const QString &deviceUni, const QString &callUni)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto voiceInterface = this->voiceInterface(modem);
    if (voiceInterface.isNull()) {
        qDebug() << Q_FUNC_INFO << "voiceInterface not found";
        return;
    }
    const auto call = voiceInterface->findCall(callUni);
    if (call.isNull()) {
        qDebug() << Q_FUNC_INFO << "call not found" << callUni;
        return;
    }
    call->hangup();
}

void ModemManagerController::sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto voiceInterface = this->voiceInterface(modem);
    if (voiceInterface.isNull()) {
        qDebug() << Q_FUNC_INFO << "voiceInterface not found";
        return;
    }
    // never try to send + as a DTMF tone
    QString safeTones = tones;
    safeTones.replace(QStringLiteral("+"), QStringLiteral("0"));
    const auto call = voiceInterface->findCall(callUni);
    if (call.isNull()) {
        qDebug() << Q_FUNC_INFO << "call not found" << callUni;
        return;
    }
    call->sendDtmf(safeTones);
}

DialerTypes::CallDataVector ModemManagerController::fetchCalls()
{
    DialerTypes::CallDataVector callDataVector;
    auto calls = m_calls;
    for (auto callObject : calls) {
        auto callData = voiceCallData(callObject);
        callDataVector.append(callData);
    }
    return callDataVector;
}

DialerTypes::CallData ModemManagerController::getCall(const QString &deviceUni, const QString &callUni)
{
    DialerTypes::CallData callData;
    auto callObject = getVoiceCallObject(deviceUni, callUni);
    if (callObject) {
        callData = voiceCallData(callObject);
    }
    return callData;
}

void ModemManagerController::deleteCall(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni) // TODO: improve deviceUni getter
    for (const QSharedPointer<ModemManager::ModemDevice> &modemDevice : ModemManager::modemDevices()) {
        const auto voiceInterface = this->voiceInterface(modemDevice);
        if (voiceInterface.isNull()) {
            qDebug() << Q_FUNC_INFO << "voiceInterface not found";
            continue;
        }
        qDebug() << Q_FUNC_INFO << "deleting voice call" << callUni << "via" << modemDevice->uni();
        voiceInterface->deleteCall(callUni);
    }
}

void ModemManagerController::onServiceAppeared()
{
}

void ModemManagerController::onServiceDisappeared()
{
}

void ModemManagerController::onModemAdded(const QString &udi)
{
    Q_UNUSED(udi);
}

void ModemManagerController::onModemRemoved(const QString &udi)
{
    Q_UNUSED(udi);
}

void ModemManagerController::init()
{
    setDeviceUniList(QStringList());
    const auto modemDevices = ModemManager::modemDevices();
    if (modemDevices.isEmpty()) {
        qWarning() << "Could not find modem devices";
        return;
    }
    for (const auto &device : modemDevices) {
        appendDeviceUni(device->uni());
        // 3GPP
        const auto modem3gppInterface = this->modem3gppInterface(device);
        if (modem3gppInterface.isNull()) {
            qDebug() << "Skipping 3GPP-related connections";
            continue;
        }
        connect(modem3gppInterface.get(), &ModemManager::Modem3gpp::countryCodeChanged, this, [this](const QString &countryCode) {
            Q_EMIT countryCodeChanged(countryCode);
        });

        // 3GPPUSSD
        const auto modem3gppUssdInterface = this->modem3gppUssdInterface(device);
        if (modem3gppInterface.isNull()) {
            qDebug() << "Skipping USSD-related connections";
            continue;
        }
        connect(modem3gppUssdInterface.get(),
                &ModemManager::Modem3gppUssd::networkNotificationChanged,
                this,
                [this, device](const QString &networkNotification) {
                    Q_EMIT ussdNotificationReceived(device->uni(), networkNotification);
                });
        connect(modem3gppUssdInterface.get(), &ModemManager::Modem3gppUssd::networkRequestChanged, this, [this, device](const QString &networkRequest) {
            Q_EMIT ussdRequestReceived(device->uni(), networkRequest);
        });
        connect(modem3gppUssdInterface.get(), &ModemManager::Modem3gppUssd::stateChanged, this, [this, device](MMModem3gppUssdSessionState state) {
            QString stateString = QStringLiteral("unknown");
            switch (state) {
            case (MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE):
                stateString = QStringLiteral("idle");
                break;
            case (MM_MODEM_3GPP_USSD_SESSION_STATE_ACTIVE):
                stateString = QStringLiteral("active");
                break;
            case (MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE):
                stateString = QStringLiteral("user-response");
                break;
            default:
                break;
            }
            Q_EMIT ussdStateChanged(device->uni(), stateString);
        });

        // Voice
        const auto voiceInterface = this->voiceInterface(device);
        if (voiceInterface.isNull()) {
            qDebug() << "Skipping Voice-related connections";
            continue;
        }
        connect(voiceInterface.get(), &ModemManager::ModemVoice::callAdded, this, [this, device, voiceInterface](const QString &callUni) {
            qDebug() << Q_FUNC_INFO << "call added, initiating";
            ModemManager::Call::Ptr call = voiceInterface->findCall(callUni);
            initAddedCall(device, call);
        });
        connect(voiceInterface.get(), &ModemManager::ModemVoice::callDeleted, this, [this, device](const QString &callUni) {
            removeCall(callUni);
            Q_EMIT callDeleted(device->uni(), callUni);
        });

        // SMS/MMS
        const auto msgManager = device->messagingInterface();
        connect(msgManager.get(), &ModemManager::ModemMessaging::messageAdded, this, [this, device, msgManager](const QString &uni, bool received) {
            ModemManager::Sms::Ptr msg = msgManager->findMessage(uni);
            Q_ASSERT(msg);
            QVariantMap message;
            message[QStringLiteral("number")] = msg->number();
            message[QStringLiteral("text")] = msg->text();
            message[QStringLiteral("data")] = msg->data();
            Q_EMIT messageAdded(device->uni(), message, received);
        });
    }

    fetchModemCalls();
}

void ModemManagerController::removeCall(const QString &callId)
{
    m_calls.erase(std::remove_if(m_calls.begin(),
                                 m_calls.end(),
                                 [callId](CallObject *callObject) {
                                     return (callObject->id() == callId);
                                 }),
                  m_calls.end());
}

QSharedPointer<ModemManager::Modem3gpp> ModemManagerController::modem3gppInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice)
{
    return modemDevice->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
}

QSharedPointer<ModemManager::Modem3gppUssd> ModemManagerController::modem3gppUssdInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice)
{
    return modemDevice->interface(ModemManager::ModemDevice::GsmUssdInterface).objectCast<ModemManager::Modem3gppUssd>();
}

QSharedPointer<ModemManager::ModemVoice> ModemManagerController::voiceInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice)
{
    return modemDevice->interface(ModemManager::ModemDevice::VoiceInterface).objectCast<ModemManager::ModemVoice>();
}

CallObject *ModemManagerController::voiceCallObject(const QSharedPointer<ModemManager::ModemDevice> &device,
                                                    const QSharedPointer<ModemManager::Call> &call,
                                                    QObject *parent)
{
    auto callObject = new CallObject(parent);
    callObject->setId(call->uni());
    callObject->setProtocol(QStringLiteral("tel"));
    callObject->setProvider(device->sim()->operatorIdentifier());
    callObject->setAccount(device->sim()->simIdentifier());
    callObject->setCommunicationWith(call->number());
    callObject->setDirection(static_cast<DialerTypes::CallDirection>(call->direction()));
    callObject->setState(static_cast<DialerTypes::CallState>(call->state()));
    callObject->setStateReason(static_cast<DialerTypes::CallStateReason>(call->stateReason()));
    callObject->setCallAttemptDuration(0);
    callObject->setStartedAt(QDateTime::currentDateTime());
    callObject->setDuration(0);
    return callObject;
}

DialerTypes::CallData ModemManagerController::voiceCallData(CallObject *callObject)
{
    DialerTypes::CallData callData;
    callData.id = callObject->id();

    callData.protocol = callObject->protocol();
    callData.provider = callObject->provider();
    callData.account = callObject->account();

    callData.communicationWith = callObject->communicationWith();
    callData.direction = callObject->direction();
    callData.state = callObject->state();
    callData.stateReason = callObject->stateReason();

    callData.callAttemptDuration = callObject->callAttemptDuration();
    callData.startedAt = callObject->startedAt();
    callData.duration = callObject->duration();
    return callData;
}

void ModemManagerController::fetchModemCalls()
{
    m_calls.clear();
    const auto modemDevices = ModemManager::modemDevices();
    if (modemDevices.isEmpty()) {
        qWarning() << "Could not find modem devices";
        return;
    }
    for (const auto &device : modemDevices) {
        const auto voiceInterface = this->voiceInterface(device);
        if (voiceInterface.isNull()) {
            qDebug() << "Skipping device without voice interface";
            continue;
        }
        const auto voiceCalls = voiceInterface->calls();
        for (const auto &call : voiceCalls) {
            initAddedCall(device, call);
        };
    };
}

CallObject *ModemManagerController::getVoiceCallObject(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    DialerTypes::CallData callData;
    auto calls = m_calls;
    for (auto callObject : calls) {
        if (callObject->id() == callUni) {
            return callObject;
        }
    }
    return nullptr;
}

void ModemManagerController::initAddedCall(const QSharedPointer<ModemManager::ModemDevice> &device, const QSharedPointer<ModemManager::Call> &call)
{
    qDebug() << Q_FUNC_INFO << "call details:" << call->direction() << call->state() << call->stateReason();
    auto voiceCallObject = this->voiceCallObject(device, call, this);
    m_calls.append(voiceCallObject);
    connect(call.get(),
            &ModemManager::Call::stateChanged,
            this,
            [this, device, call, voiceCallObject](MMCallState oldState, MMCallState newState, MMCallStateReason reason) {
                Q_UNUSED(oldState);
                qDebug() << Q_FUNC_INFO << "call state changed, initiating";
                auto callDirection = static_cast<DialerTypes::CallDirection>(call->direction());
                auto callState = static_cast<DialerTypes::CallState>(newState);
                auto callReason = static_cast<DialerTypes::CallStateReason>(reason);
                if (voiceCallObject) {
                    voiceCallObject->onCallStateChanged(device->uni(), call->uni(), callDirection, callState, callReason);
                }
                Q_EMIT callStateChanged(voiceCallData(voiceCallObject));
            });
    Q_EMIT callAdded(device->uni(),
                     call->uni(),
                     static_cast<DialerTypes::CallDirection>(call->direction()),
                     static_cast<DialerTypes::CallState>(call->state()),
                     static_cast<DialerTypes::CallStateReason>(call->stateReason()),
                     call->number());
    if ((call->state() == MM_CALL_STATE_UNKNOWN) && call->direction() == MM_CALL_DIRECTION_OUTGOING) {
        qDebug() << Q_FUNC_INFO << "automatically starting outgoing call";
        call->start();
    }
}
