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
        return;
    }
    const auto modem3gppUssdInterface = _modem3gppUssdInterface(modem);
    QDBusPendingReply<QString> reply = modem3gppUssdInterface->initiate(command);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        return;
    }
    Q_EMIT ussdInitiateComplete(deviceUni, reply.value());
}

void ModemManagerController::ussdRespond(const QString &deviceUni, const QString &reply)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto modem3gppUssdInterface = _modem3gppUssdInterface(modem);
    modem3gppUssdInterface->respond(reply);
}

void ModemManagerController::ussdCancel(const QString &deviceUni)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto modem3gppUssdInterface = _modem3gppUssdInterface(modem);
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
    const auto voiceInterface = _voiceInterface(modem);
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
    const auto voiceInterface = _voiceInterface(modem);
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
    const auto voiceInterface = _voiceInterface(modem);
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
    const auto voiceInterface = _voiceInterface(modem);
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
    const auto voiceInterface = _voiceInterface(modem);
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
    auto calls = _calls;
    for (auto callObject : calls) {
        auto callData = _voiceCallData(callObject);
        callDataVector.append(callData);
    }
    return callDataVector;
}

DialerTypes::CallData ModemManagerController::getCall(const QString &deviceUni, const QString &callUni)
{
    DialerTypes::CallData callData;
    auto callObject = _getVoiceCallObject(deviceUni, callUni);
    if (callObject) {
        callData = _voiceCallData(callObject);
    }
    return callData;
}

void ModemManagerController::deleteCall(const QString &deviceUni, const QString &callUni)
{
    const auto modem = ModemManager::findModemDevice(deviceUni);
    if (modem.isNull()) {
        qDebug() << Q_FUNC_INFO << "deviceUni not found:" << deviceUni;
        return;
    }
    const auto voiceInterface = _voiceInterface(modem);
    if (voiceInterface.isNull()) {
        qDebug() << Q_FUNC_INFO << "voiceInterface not found";
        return;
    }
    qDebug() << Q_FUNC_INFO << "deleting call" << deviceUni << callUni;
    voiceInterface->deleteCall(callUni);
}

int ModemManagerController::getCallDuration()
{
    auto calls = _calls;
    for (auto callObject : calls) {
        auto callData = _voiceCallData(callObject);
        // TODO (Alexander Trofimov): Now returning just the first found callObject
        // Need to add deviceUni and callUni and fined the call by id
        return callData.duration;
    }

    return 0;
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
        const auto modem3gppInterface = _modem3gppInterface(device);
        if (modem3gppInterface.isNull()) {
            qDebug() << "Skipping 3GPP-related connections";
            continue;
        }
        connect(modem3gppInterface.get(), &ModemManager::Modem3gpp::countryCodeChanged, this, [this](const QString &countryCode) {
            Q_EMIT countryCodeChanged(countryCode);
        });

        // 3GPPUSSD
        const auto modem3gppUssdInterface = _modem3gppUssdInterface(device);
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
        const auto voiceInterface = _voiceInterface(device);
        if (voiceInterface.isNull()) {
            qDebug() << "Skipping Voice-related connections";
            continue;
        }
        connect(voiceInterface.get(), &ModemManager::ModemVoice::callAdded, this, [this, device, voiceInterface](const QString &callUni) {
            qDebug() << Q_FUNC_INFO << "call added, initiating";
            ModemManager::Call::Ptr call = voiceInterface->findCall(callUni);
            _initAddedCall(device, call);
        });
        connect(voiceInterface.get(), &ModemManager::ModemVoice::callDeleted, this, [this, device](const QString &callUni) {
            _removeCall(callUni);
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

    _fetchModemCalls();
}

void ModemManagerController::_removeCall(const QString &callId)
{
    _calls.erase(std::remove_if(_calls.begin(),
                                _calls.end(),
                                [callId](CallObject *callObject) {
                                    return (callObject->id() == callId);
                                }),
                 _calls.end());
}

QSharedPointer<ModemManager::Modem3gpp> ModemManagerController::_modem3gppInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice)
{
    return modemDevice->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
}

QSharedPointer<ModemManager::Modem3gppUssd> ModemManagerController::_modem3gppUssdInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice)
{
    return modemDevice->interface(ModemManager::ModemDevice::GsmUssdInterface).objectCast<ModemManager::Modem3gppUssd>();
}

QSharedPointer<ModemManager::ModemVoice> ModemManagerController::_voiceInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice)
{
    return modemDevice->interface(ModemManager::ModemDevice::VoiceInterface).objectCast<ModemManager::ModemVoice>();
}

CallObject *ModemManagerController::_voiceCallObject(const QSharedPointer<ModemManager::ModemDevice> &device,
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

DialerTypes::CallData ModemManagerController::_voiceCallData(CallObject *callObject)
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

void ModemManagerController::_fetchModemCalls()
{
    _calls.clear();
    const auto modemDevices = ModemManager::modemDevices();
    if (modemDevices.isEmpty()) {
        qWarning() << "Could not find modem devices";
        return;
    }
    for (const auto &device : modemDevices) {
        const auto voiceInterface = this->_voiceInterface(device);
        if (voiceInterface.isNull()) {
            qDebug() << "Skipping device without voice interface";
            continue;
        }
        const auto voiceCalls = voiceInterface->calls();
        for (const auto &call : voiceCalls) {
            _initAddedCall(device, call);
        };
    };
}

CallObject *ModemManagerController::_getVoiceCallObject(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    DialerTypes::CallData callData;
    auto calls = _calls;
    for (auto callObject : calls) {
        if (callObject->id() == callUni) {
            return callObject;
        }
    }
    return nullptr;
}

void ModemManagerController::_initAddedCall(const QSharedPointer<ModemManager::ModemDevice> &device, const QSharedPointer<ModemManager::Call> &call)
{
    qDebug() << Q_FUNC_INFO << "call details:" << call->direction() << call->state() << call->stateReason();
    auto voiceCallObject = _voiceCallObject(device, call, this);
    _calls.append(voiceCallObject);
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
                Q_EMIT callStateChanged(device->uni(), call->uni(), callDirection, callState, callReason);
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
