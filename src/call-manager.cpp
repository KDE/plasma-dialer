/*
    Copyright (C) 2012 George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "call-manager.h"
#include "dialerutils.h"
#include "qpulseaudioengine.h"

#include <QTimer>

#include <KNotification>
#include <KLocalizedString>

static void enable_earpiece()
{
    QPulseAudioEngine::instance()->setCallMode(CallActive, AudioModeEarpiece);
}


static void enable_normal()
{
    QTimer* timer = new QTimer();
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=](){
        QPulseAudioEngine::instance()->setMicMute(false);
        QPulseAudioEngine::instance()->setCallMode(CallEnded, AudioModeWiredOrSpeaker);
        timer->deleteLater();
    });
    timer->start(2000);
}

static void enable_speaker()
{
    QPulseAudioEngine::instance()->setCallMode(CallActive, AudioModeSpeaker);
}

constexpr int MISSED_CALL_REASON = 5;
constexpr int CALL_DURATION_UPDATE_DELAY = 1000;

struct CallManager::Private
{
    Tp::CallChannelPtr callChannel;
    DialerUtils *dialerUtils;
    KNotification *callsNotification;
    uint missedCalls;
    QTimer *callTimer;
};

CallManager::CallManager(const Tp::CallChannelPtr &callChannel, DialerUtils *dialerUtils, QObject *parent)
    : QObject(parent), d(new Private)
{

    d->dialerUtils = dialerUtils;
    d->callChannel = callChannel;
    connect(callChannel.data(), &Tp::CallChannel::callStateChanged,
            this, &CallManager::onCallStateChanged);

    connect(d->dialerUtils, &DialerUtils::acceptCall, this, &CallManager::onCallAccepted);
    connect(d->dialerUtils, &DialerUtils::rejectCall, this, &CallManager::onCallRejected);
    connect(d->dialerUtils, &DialerUtils::hangUp, this, &CallManager::onHangUpRequested);
    connect(d->callChannel.data(), &Tp::CallChannel::invalidated, this, [=]() {
        qDebug() << "Channel invalidated";
        d->dialerUtils->setCallState(DialerUtils::CallState::Idle);
        d->dialerUtils->emitCallEnded();
        deleteLater();
    });
    d->callsNotification = nullptr;
    d->callTimer = nullptr;

    //bring us up-to-date with the current call state
    onCallStateChanged(d->callChannel->callState());
}

CallManager::~CallManager()
{
    qDebug() << "Deleting CallManager";

}

void CallManager::onCallStateChanged(Tp::CallState state)
{
    qDebug() << "new call state:" << state;

    if (d->callChannel->targetContact()) {
        d->dialerUtils->setCallContactAlias(d->callChannel->targetContact()->alias());
        d->dialerUtils->setCallContactNumber(d->callChannel->targetContact()->id());
    }

    if (d->callChannel->isValid()) {
        d->dialerUtils->setIsIncomingCall(!d->callChannel->isRequested());
    }

    switch (state) {
    case Tp::CallStatePendingInitiator:
        Q_ASSERT(d->callChannel->isRequested());
        (void) d->callChannel->accept();
        break;
    case Tp::CallStateInitialising:
        if (d->callChannel->isRequested()) {
            d->dialerUtils->setCallState(DialerUtils::CallState::Dialing);
            qDebug() << "CallStateInitialising";
            enable_earpiece();
        } else {
            qDebug() << "Call is initialising";
        }
        break;
    case Tp::CallStateInitialised:
        if (d->callChannel->isRequested()) {
            d->dialerUtils->setCallState(DialerUtils::CallState::Dialing);
            qDebug() << "Call state initialized";
            enable_earpiece();
        } else {
            qDebug() << "Call state initialized #2";
            enable_earpiece();
            d->dialerUtils->setCallState(DialerUtils::CallState::Incoming);
        }
        break;
    case Tp::CallStateAccepted:
        if (d->callChannel->isRequested()) {
            d->dialerUtils->setCallState(DialerUtils::CallState::Answered);
        }
        break;
    case Tp::CallStateActive:
        d->dialerUtils->setCallState(DialerUtils::CallState::Active);
        d->callTimer = new QTimer(this);
        connect(d->callTimer, &QTimer::timeout, d->callTimer, [=]() {
            d->dialerUtils->setCallDuration(d->dialerUtils->callDuration() + 1);
        });
        d->callTimer->start(CALL_DURATION_UPDATE_DELAY);
        qDebug() << "call state active";
        enable_earpiece();
        break;
    case Tp::CallStateEnded:
        d->dialerUtils->setCallState(DialerUtils::CallState::Ended);
        qDebug() << "Call ended";
        enable_normal();
        //FIXME this is defined in the spec, but try to find a proper enum value for it
        if (d->callChannel->callStateReason().reason == MISSED_CALL_REASON) {
            qDebug() << "Adding notification";
            d->missedCalls++;
            if (!d->callsNotification) {
                d->callsNotification = new KNotification(QStringLiteral("callMissed"), KNotification::Persistent, nullptr);
            }
            d->callsNotification->setComponentName(QStringLiteral("plasma_dialer"));
            d->callsNotification->setIconName(QStringLiteral("call-start"));
            if (d->missedCalls == 1) {
                d->callsNotification->setTitle(i18n("Missed call from %1", d->callChannel->targetContact()->alias()));
                d->callsNotification->setText(QTime::currentTime().toString(QStringLiteral("HH:mm")));
            } else {
                d->callsNotification->setTitle(i18n("%1 calls missed", d->missedCalls));
                d->callsNotification->setText(i18n("Last call: %1", QTime::currentTime().toString(QStringLiteral("HH:mm"))));
            }

            if (d->missedCalls == 1) {
                d->callsNotification->sendEvent();
            } else {
                d->callsNotification->update();
            }
        }

        if (d->callTimer) {
            d->callTimer->stop();
            d->callTimer->deleteLater();
            d->callTimer = nullptr;
        }
        break;
    default:
        Q_ASSERT(false);
    }
}

void CallManager::onCallAccepted()
{
    (void) d->callChannel->accept();
}

void CallManager::onCallRejected()
{
    (void) d->callChannel->hangup(Tp::CallStateChangeReasonRejected, TP_QT_ERROR_REJECTED); // clazy:exclude=qstring-allocations
}

void CallManager::onHangUpRequested()
{
    if (d->callChannel && d->callChannel->isValid()) {
        qDebug() << "Hanging up";
        Tp::PendingOperation *op = d->callChannel->hangup();
        connect(op, &Tp::PendingOperation::finished, [=]() {
            if (op->isError()) {
                qWarning() << "Unable to hang up:" << op->errorMessage();
            }
            d->callChannel->requestClose();
        });
    }
}

