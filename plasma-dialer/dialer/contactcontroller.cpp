/*
 *   SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "contactcontroller.h"

#include <KContacts/VCardConverter>

#include <KConfigGroup>
#include <KWindowConfig>
#include <QDebug>

const static KContacts::VCardConverter converter;

ContactController::ContactController()
    : m_dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation)
{
}

QByteArray ContactController::addresseeToVCard(const KContacts::Addressee &addressee)
{
    return converter.createVCard(addressee);
}

KContacts::Addressee ContactController::emptyAddressee()
{
    return {};
}

KContacts::PhoneNumber ContactController::createPhoneNumber(const QString &number)
{
    KContacts::PhoneNumber pn(number);
    pn.setType(KContacts::PhoneNumber::Cell);
    return pn;
}
