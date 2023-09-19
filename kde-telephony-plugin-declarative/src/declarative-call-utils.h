// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include "callutilsinterface.h"

class DeclarativeCallUtils : public org::kde::telephony::CallUtils
{
    Q_OBJECT
public:
    DeclarativeCallUtils(QObject *parent = nullptr);

    Q_INVOKABLE void accept(const QString &deviceUni, const QString &callUni);
    Q_INVOKABLE void dial(const QString &deviceUni, const QString &number);
    Q_INVOKABLE QString formatNumber(const QString &number);
    Q_INVOKABLE void hangUp(const QString &deviceUni, const QString &callUni);
    Q_INVOKABLE void sendDtmf(const QString &deviceUni, const QString &callUni, const QString &tones);
};
