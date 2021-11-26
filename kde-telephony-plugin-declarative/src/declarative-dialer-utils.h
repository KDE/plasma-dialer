// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>

#include "dialerutilsinterface.h"

class DeclarativeDialerUtils : public org::kde::telephony::DialerUtils
{
    Q_OBJECT
public:
    DeclarativeDialerUtils(QObject *parent = nullptr);

    Q_INVOKABLE void setSpeakerMode(bool enabled);
    Q_INVOKABLE void setMute(bool muted);
};
