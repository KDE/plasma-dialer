// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL
#pragma once

#include <kTelephonyMetaTypes/dialer-types.h>

class ModemController : public QObject
{
    Q_OBJECT

public:
    explicit ModemController(QObject *parent = nullptr);
Q_SIGNALS:
    // Modem/3GPP
    void deviceUniListChanged(const QStringList &deviceUniList);
    void countryCodeChanged(const QString &countryCode);
    // SMS/MMS
    void messageAdded(const QString &deviceUni, const QVariantMap &message, bool received);
    // USSD
    void ussdNotificationReceived(const QString &deviceUni, const QString &message);
    void ussdRequestReceived(const QString &deviceUni, const QString &message);
    void ussdInitiateComplete(const QString &deviceUni, const QString &command);
    void ussdStateChanged(const QString &deviceUni, const QString &state);
    // Calls
    void callAdded(const QString &deviceUni,
                   const QString &callUni,
                   const DialerTypes::CallDirection &callDirection,
                   const DialerTypes::CallState &callState,
                   const DialerTypes::CallStateReason &callStateReason,
                   const QString communicationWith);
    void callDeleted(const QString &deviceUni, const QString &callUni);
    void callStateChanged(const QString &deviceUni,
                          const QString &callUni,
                          const DialerTypes::CallDirection &callDirection,
                          const DialerTypes::CallState &callState,
                          const DialerTypes::CallStateReason &callStateReason);

public Q_SLOTS:
    // Device
    QStringList deviceUniList();
    void appendDeviceUni(const QString &deviceUni);
    void setDeviceUniList(const QStringList &newDeviceUniList);
    virtual QString equipmentIdentifier(const QString &deviceUni);
    // USSD
    virtual void ussdInitiate(const QString &deviceUni, const QString &command);
    virtual void ussdRespond(const QString &deviceUni, const QString &reply);
    virtual void ussdCancel(const QString &deviceUni);
    // Calls
    virtual QString callNumber(const QString &deviceUni, const QString &callUni);
    virtual void createCall(const QString &deviceUni, const QString &callUni);
    virtual void acceptCall(const QString &deviceUni, const QString &callUni);
    virtual void hangUp(const QString &deviceUni, const QString &callUni);
    virtual void sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones);
    virtual DialerTypes::CallDataVector fetchCalls();
    virtual DialerTypes::CallData getCall(const QString &deviceUni, const QString &callUni);
    virtual void deleteCall(const QString &deviceUni, const QString &callUni);
    virtual int getCallDuration();

private:
    QStringList _deviceUniList;
};
