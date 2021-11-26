// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-dialer-utils.h"

DeclarativeDialerUtils::DeclarativeDialerUtils(QObject *parent)
    : org::kde::telephony::DialerUtils(QString::fromLatin1(staticInterfaceName()),
                                       QStringLiteral("/org/kde/telephony/DialerUtils/tel/mm"),
                                       QDBusConnection::sessionBus(),
                                       parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate DialerUtils interface";
        return;
    }
}

void DeclarativeDialerUtils::setSpeakerMode(bool enabled)
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::setSpeakerMode(enabled);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeDialerUtils::setMute(bool muted)
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::setMute(muted);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}
