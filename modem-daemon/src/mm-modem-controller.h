// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL
#pragma once

#include "modem-controller.h"

#include <ModemManagerQt/Call>
#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/Modem3Gpp>
#include <ModemManagerQt/Modem3GppUssd>
#include <ModemManagerQt/ModemMessaging>
#include <ModemManagerQt/ModemVoice>
#include <ModemManagerQt/Sms>

#include "call-object.h"

class ModemManagerController : public ModemController
{
    Q_OBJECT

public:
    explicit ModemManagerController(QObject *parent = nullptr);

public Q_SLOTS:
    // Device
    QString equipmentIdentifier(const QString &deviceUni) override;
    // USSD
    void ussdInitiate(const QString &deviceUni, const QString &command) override;
    void ussdRespond(const QString &deviceUni, const QString &reply) override;
    void ussdCancel(const QString &deviceUni) override;
    // Calls
    QString callNumber(const QString &deviceUni, const QString &callUni) override;
    void createCall(const QString &deviceUni, const QString &callUni) override;
    void acceptCall(const QString &deviceUni, const QString &callUni) override;
    void hangUp(const QString &deviceUni, const QString &callUni) override;
    void sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones) override;
    DialerTypes::CallDataVector fetchCalls() override;
    DialerTypes::CallData getCall(const QString &deviceUni, const QString &callUni) override;
    void deleteCall(const QString &deviceUni, const QString &callUni) override;
    int getCallDuration() override;

private Q_SLOTS:
    void onServiceAppeared();
    void onServiceDisappeared();
    void onModemAdded(const QString &udi);
    void onModemRemoved(const QString &udi);

private:
    void init();

    QVector<CallObject *> _calls;
    void _removeCall(const QString &callId);

    QSharedPointer<ModemManager::Modem3gpp> _modem3gppInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice);

    QSharedPointer<ModemManager::Modem3gppUssd> _modem3gppUssdInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice);

    QSharedPointer<ModemManager::ModemVoice> _voiceInterface(const QSharedPointer<ModemManager::ModemDevice> modemDevice);

    CallObject *
    _voiceCallObject(const QSharedPointer<ModemManager::ModemDevice> &device, const QSharedPointer<ModemManager::Call> &call, QObject *parent = nullptr);

    DialerTypes::CallData _voiceCallData(CallObject *callObject);

    void _fetchModemCalls();
    CallObject *_getVoiceCallObject(const QString &deviceUni, const QString &callUni);
    void _initAddedCall(const QSharedPointer<ModemManager::ModemDevice> &device, const QSharedPointer<ModemManager::Call> &call);
};
