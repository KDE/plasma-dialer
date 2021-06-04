// SPDX-FileCopyrightText: 2020-2021 Bhushan Shah <bshah@kde.org>
// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "callchannelapprover.h"

#include <KLocalizedString>

#include <KPeople/PersonData>

#include <TelepathyQt/CallChannel>
#include <TelepathyQt/ChannelClassSpec>
#include <TelepathyQt/ClientRegistrar>
#include <TelepathyQt/PendingReady>

#include "contactmapper.h"

CallChannelApprover::CallChannelApprover(const Tp::CallChannelPtr &channel, QObject *parent)
    : ChannelApprover(parent)
{
    if (!channel.isNull()) {
        Tp::PendingReady *pendingReady = channel->becomeReady(Tp::Features() << Tp::CallChannel::FeatureCore << Tp::CallChannel::FeatureCallState);
        m_Channels[pendingReady] = channel;
        connect(pendingReady, &Tp::PendingOperation::finished, this, &CallChannelApprover::onChannelReady);
    }
}

CallChannelApprover::~CallChannelApprover()
{
    deleteLater();
}

void CallChannelApprover::onChannelReady(Tp::PendingOperation *op)
{
    auto *pendingReady = qobject_cast<Tp::PendingReady *>(op);
    if (!pendingReady || !m_Channels.contains(pendingReady)) {
        qWarning() << "Pending operation is not tracked" << op;
        return;
    }

    Tp::ChannelPtr channel = m_Channels.value(pendingReady);
    Tp::CallChannelPtr callChannel = Tp::CallChannelPtr::dynamicCast(channel);

    if (channel->initiatorContact() != channel->connection()->selfContact() && callChannel->callState() == Tp::CallStateInitialised
        && !callChannel->isRequested()) {
        callChannel->setRinging();
    }

    KPeople::PersonData person(ContactMapper::instance().uriForNumber(callChannel->targetContact()->alias()));

    const QString callerDisplayName = !person.name().isEmpty() ? person.name() : callChannel->targetContact()->alias();

    QStringList actions;
    actions << i18n("Accept") << i18n("Reject");
    if (!m_ringingNotification) {
        m_ringingNotification = new KNotification(QStringLiteral("ringing"), KNotification::Persistent | KNotification::LoopSound, nullptr);
    }
    m_ringingNotification->setUrgency(KNotification::CriticalUrgency);
    m_ringingNotification->setComponentName(QStringLiteral("plasma_dialer"));
    m_ringingNotification->setPixmap(person.photo());
    m_ringingNotification->setTitle(i18n("Incoming call"));
    m_ringingNotification->setText(callerDisplayName);
    // this will be used by the notification applet to show custom notification UI
    // with swipe decision.
    m_ringingNotification->setHint(QStringLiteral("category"), "x-kde.incoming-call");
    m_ringingNotification->setActions(actions);
    m_ringingNotification->sendEvent();

    QDBusMessage wakeupCall = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                             QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("wakeup"));
    QDBusConnection::sessionBus().call(wakeupCall);

    connect(callChannel.data(), &Tp::CallChannel::callStateChanged, this, [=, this](Tp::CallState state) {
        if (state == Tp::CallStateEnded) {
            m_ringingNotification->close();
        }
    });
    void (KNotification::*activation)(unsigned int) = &KNotification::activated;
    connect(m_ringingNotification, activation, this, &CallChannelApprover::onNotificationAction);
}

void CallChannelApprover::onNotificationAction(unsigned int action)
{
    switch (action) {
    case 1:
        // call accepted
        emit channelAccepted();
        break;
    case 2:
        // call rejected
        emit channelRejected();
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}
