// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-call-utils.h"

DeclarativeCallUtils::DeclarativeCallUtils(QObject *parent)
    : org::kde::telephony::CallUtils(QString::fromLatin1(staticInterfaceName()),
                                     QStringLiteral("/org/kde/telephony/CallUtils/tel/mm"),
                                     QDBusConnection::sessionBus(),
                                     parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallUtils interface";
        return;
    }
}

void DeclarativeCallUtils::fetchCalls()
{
    QDBusPendingReply<> reply = org::kde::telephony::CallUtils::fetchCalls();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeCallUtils::accept(const QString &deviceUni, const QString &callUni)
{
    QDBusPendingReply<> reply = org::kde::telephony::CallUtils::accept(deviceUni, callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeCallUtils::dial(const QString &deviceUni, const QString &number)
{
    QDBusPendingReply<> reply = org::kde::telephony::CallUtils::dial(deviceUni, number);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeCallUtils::hangUp(const QString &deviceUni, const QString &callUni)
{
    QDBusPendingReply<> reply = org::kde::telephony::CallUtils::hangUp(deviceUni, callUni);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeCallUtils::sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones)
{
    QDBusPendingReply<> reply = org::kde::telephony::CallUtils::sendDtmf(deviceUni, callUni, tones);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

QString DeclarativeCallUtils::formatNumber(const QString &number)
{
    QDBusPendingReply<QString> reply = org::kde::telephony::CallUtils::formatNumber(number);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
    return reply;
}
