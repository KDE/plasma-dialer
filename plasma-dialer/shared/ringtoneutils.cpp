// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ringtoneutils.h"
#include <QDebug>
#include <QStandardPaths>

using namespace Qt::StringLiterals;

RingtoneUtils::RingtoneUtils(QObject *parent)
    : QObject(parent)
{
}

bool RingtoneUtils::systemHasPlasmaMobileSoundTheme() const
{
    const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"sounds/plasma-mobile"_s, QStandardPaths::LocateDirectory);
    return !path.isEmpty();
}

bool RingtoneUtils::systemHasOceanSoundTheme() const
{
    const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"sounds/ocean"_s, QStandardPaths::LocateDirectory);
    return !path.isEmpty();
}

QString RingtoneUtils::getSoundTheme() const
{
    if (systemHasPlasmaMobileSoundTheme()) {
        return u"plasma-mobile"_s;
    }
    if (systemHasOceanSoundTheme()) {
        return u"ocean"_s;
    }
    return u"freedesktop"_s;
}

QString RingtoneUtils::getDefaultRingtoneLocation() const
{
    const QString theme = getSoundTheme();
    return QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"sounds/"_s + theme + u"/stereo/phone-incoming-call.oga"_s);
}

bool RingtoneUtils::soundInSoundTheme(const QString &soundPath)
{
    const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"sounds/"_s + getSoundTheme(), QStandardPaths::LocateDirectory);
    qDebug() << path << soundPath;
    return soundPath.startsWith(path);
}

#include "moc_ringtoneutils.cpp"
