// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include "ussdutilsinterface.h"

class DeclarativeUssdUtils : public org::kde::telephony::UssdUtils
{
    Q_OBJECT
public:
    DeclarativeUssdUtils(QObject *parent = nullptr);

    Q_INVOKABLE void initiate(const QString &deviceUni, const QString &command);
    Q_INVOKABLE void respond(const QString &deviceUni, const QString &reply);
    Q_INVOKABLE void cancel(const QString &deviceUni);
};
