// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-dialer-utils.h"

DeclarativeDialerUtils::DeclarativeDialerUtils(QObject *parent)
    : org::kde::telephony::DialerUtils(QString::fromLatin1(staticInterfaceName()),
                                       QStringLiteral("/org/kde/telephony/DialerUtils/tel/mm"),
                                       QDBusConnection::sessionBus(),
                                       parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate DialerUtils interface";
        return;
    }

    connect(this, &org::kde::telephony::DialerUtils::muteChanged, this, &DeclarativeDialerUtils::_onMuteChanged);
    connect(this, &org::kde::telephony::DialerUtils::speakerModeChanged, this, &DeclarativeDialerUtils::_onSpeakerModeChanged);

    _fetchMute();
    _fetchSpeakerMode();
}

void DeclarativeDialerUtils::setSpeakerMode(bool enabled)
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::setSpeakerMode(enabled);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeDialerUtils::setMute(bool muted)
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::setMute(muted);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

bool DeclarativeDialerUtils::mute() const
{
    return _mute;
}

bool DeclarativeDialerUtils::speakerMode() const
{
    return _speakerMode;
}

void DeclarativeDialerUtils::_fetchMute()
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::fetchMute();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeDialerUtils::_fetchSpeakerMode()
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::fetchSpeakerMode();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    }
}

void DeclarativeDialerUtils::_onMuteChanged(bool mute)
{
    if (_mute != mute) {
        _mute = mute;
        Q_EMIT declarativeMuteChanged(_mute);
    }
}

void DeclarativeDialerUtils::_onSpeakerModeChanged(bool speakerMode)
{
    if (_speakerMode != speakerMode) {
        _speakerMode = speakerMode;
        Q_EMIT declarativeSpeakerModeChanged(_speakerMode);
    }
}

void DeclarativeDialerUtils::syncSettings()
{
    QDBusPendingReply<> reply = org::kde::telephony::DialerUtils::syncSettings();
}
