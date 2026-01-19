// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dialer-utils.h"

#include <QDebug>
#include <QFile>
#include <QStandardPaths>

#include <KConfig>
#include <KConfigGroup>
#include <KNotifyConfig>

#include "config.h"

DialerUtils::DialerUtils(QObject *parent)
    : QObject(parent)
{
}

bool DialerUtils::mute()
{
    return m_mute;
}

bool DialerUtils::speakerMode()
{
    return m_speakerMode;
}

void DialerUtils::setSpeakerMode(bool enabled)
{
    if (m_speakerMode != enabled) {
        m_speakerMode = enabled;
        Q_EMIT speakerModeChanged(m_speakerMode);
    }
}

void DialerUtils::fetchMute()
{
    Q_EMIT muteRequested();
}

void DialerUtils::fetchSpeakerMode()
{
    Q_EMIT speakerModeRequested();
}

void DialerUtils::setMute(bool muted)
{
    if (m_mute != muted) {
        m_mute = muted;
        Q_EMIT muteChanged(m_mute);
    }
}

void DialerUtils::syncSettings()
{
    Config::self()->load();
    updateRingtoneConfig();
}

void DialerUtils::updateRingtoneConfig()
{
    QString customRingtone = Config::self()->customRingtone();

    // Validate the file exists
    if (!customRingtone.isEmpty() && !QFile::exists(customRingtone)) {
        qWarning() << "Custom ringtone file not found:" << customRingtone;
        customRingtone.clear();
        Config::self()->setCustomRingtone(QString());
        Config::self()->save();
    }

    const QString userNotifyRcPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/plasma-dialer.notifyrc");

    KConfig userConfig(userNotifyRcPath, KConfig::SimpleConfig);
    KConfigGroup ringingGroup(&userConfig, QStringLiteral("Event/ringing"));
    KConfigGroup ringingWithoutPopupGroup(&userConfig, QStringLiteral("Event/ringing-without-popup"));

    if (customRingtone.isEmpty()) {
        // Remove override to use system default
        ringingGroup.deleteEntry("Sound");
        ringingWithoutPopupGroup.deleteEntry("Sound");
    } else {
        // Set custom sound
        ringingGroup.writePathEntry("Sound", customRingtone);
        ringingWithoutPopupGroup.writePathEntry("Sound", customRingtone);
    }

    userConfig.sync();

    // Force KNotification to reload the config
    KNotifyConfig::reparseSingleConfiguration(QStringLiteral("plasma-dialer"));
}
