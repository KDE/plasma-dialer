// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-contact-utils.h"

#include "callutilsinterface.h"

DeclarativeContactUtils::DeclarativeContactUtils(QObject *parent)
    : org::kde::telephony::ContactUtils(QString::fromLatin1(staticInterfaceName()),
                                        QStringLiteral("/org/kde/telephony/ContactUtils/tel/mm"),
                                        QDBusConnection::sessionBus(),
                                        parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate ContactUtils interface";
        return;
    }
}

QString DeclarativeContactUtils::displayString(const QString &contact)
{
    QString displayString = contact;
    return displayString;
}
