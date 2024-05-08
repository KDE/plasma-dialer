// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "modem-controller.h"

#include <QDebug>

ModemController::ModemController(QObject *parent)
    : QObject(parent)
{
}

QString ModemController::protocol()
{
    return QStringLiteral("tel");
}

QString ModemController::subsystem()
{
    return QStringLiteral("native");
}

QStringList ModemController::deviceUniList()
{
    return m_deviceUniList;
}

void ModemController::appendDeviceUni(const QString &deviceUni)
{
    if (!deviceUniList().contains(deviceUni)) {
        qDebug() << "found new device:" << deviceUni;
        m_deviceUniList.append(deviceUni);
        Q_EMIT deviceUniListChanged(m_deviceUniList);
    }
}

void ModemController::setDeviceUniList(const QStringList &newDeviceUniList)
{
    if (m_deviceUniList == newDeviceUniList)
        return;
    m_deviceUniList = newDeviceUniList;
    Q_EMIT deviceUniListChanged(m_deviceUniList);
}

QString ModemController::equipmentIdentifier(const QString &deviceUni)
{
    Q_UNUSED(deviceUni);
    return QString();
}

QString ModemController::deviceUni(const QString &equipmentIdentifier)
{
    // TODO: improve deviceUni getter
    for (const auto &deviceUni : qAsConst(m_deviceUniList)) {
        if (equipmentIdentifier == ModemController::equipmentIdentifier(deviceUni)) {
            return deviceUni;
        }
    }

    return QString();
}

void ModemController::ussdInitiate(const QString &deviceUni, const QString &command)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(command);
}

void ModemController::ussdRespond(const QString &deviceUni, const QString &reply)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(reply);
}

void ModemController::ussdCancel(const QString &deviceUni)
{
    Q_UNUSED(deviceUni);
}

QString ModemController::callNumber(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    return QString();
}

void ModemController::createCall(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
}

void ModemController::acceptCall(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
}

void ModemController::hangUp(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
}

void ModemController::sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    Q_UNUSED(tones);
}

DialerTypes::CallDataVector ModemController::fetchCalls()
{
    return DialerTypes::CallDataVector();
}

DialerTypes::CallData ModemController::getCall(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    DialerTypes::CallData callData;
    return callData;
}

void ModemController::deleteCall(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
}
