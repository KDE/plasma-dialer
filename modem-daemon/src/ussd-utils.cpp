// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "ussd-utils.h"

UssdUtils::UssdUtils(QObject *parent)
    : QObject(parent)
{
}

void UssdUtils::initiate(const QString &deviceUni, const QString &command)
{
    Q_EMIT initiated(deviceUni, command);
}

void UssdUtils::respond(const QString &deviceUni, const QString &reply)
{
    Q_EMIT responded(deviceUni, reply);
}

void UssdUtils::cancel(const QString &deviceUni)
{
    Q_EMIT cancelled(deviceUni);
}
