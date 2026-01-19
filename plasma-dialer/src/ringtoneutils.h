// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <qqmlregistration.h>

class RingtoneUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit RingtoneUtils(QObject *parent = nullptr);

    Q_INVOKABLE QString getSoundTheme() const;
    Q_INVOKABLE QString getDefaultRingtoneLocation() const;
    Q_INVOKABLE bool systemHasPlasmaMobileSoundTheme() const;
    Q_INVOKABLE bool systemHasOceanSoundTheme() const;
    Q_INVOKABLE bool soundInSoundTheme(const QString &soundPath);
};
