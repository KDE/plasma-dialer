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
    connect(this, &org::kde::telephony::DialerUtils::muteChanged, this, &DeclarativeDialerUtils::muteChanged);
    connect(this, &org::kde::telephony::DialerUtils::speakerModeChanged, this, &DeclarativeDialerUtils::speakerModeChanged);
}

void DeclarativeDialerUtils::syncSettings()
{
    org::kde::telephony::DialerUtils::syncSettings();
}
