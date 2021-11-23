// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>

class DialerUtils : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.telephony.DialerUtils")

public:
    DialerUtils(QObject *parent = nullptr);

public Q_SLOTS:
    Q_INVOKABLE void setSpeakerMode(bool enabled);
    Q_INVOKABLE void setMute(bool muted);

Q_SIGNALS:
    void speakerModeChanged(bool enabled);
    void muteChanged(bool muted);
    void missedCallsActionTriggered();
};
