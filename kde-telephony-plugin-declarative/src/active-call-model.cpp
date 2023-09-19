// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "active-call-model.h"

constexpr int CALL_DURATION_UPDATE_DELAY = 1000;

ActiveCallModel::ActiveCallModel(QObject *parent)
    : CallModel(parent)
{
}

void ActiveCallModel::sendDtmf(const QString &tones)
{
    QString deviceUni;
    QString callUni;
    _callUtils->sendDtmf(deviceUni, callUni, tones);
}

void ActiveCallModel::dial(const QString &deviceUni, const QString &number)
{
    _callUtils->dial(deviceUni, number);
}

QString ActiveCallModel::activeCallUni()
{
    QString activeCallUni;
    if (_calls.size() < 1) {
        qDebug() << Q_FUNC_INFO << "empty active calls list";
        return activeCallUni;
    }
    for (int i = 0; i < _calls.size(); i++) {
        const auto call = _calls.at(i);
        if (call.state != DialerTypes::CallState::Terminated) {
            return call.id;
        }
    }
    return activeCallUni;
}

QVariant ActiveCallModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    switch (role) {
    case Roles::EventRole:
        return _calls[row].id;
    case Roles::ProtocolRole:
        return _calls[row].protocol;
    case Roles::AccountRole:
        return _calls[row].account;
    case Roles::ProviderRole:
        return _calls[row].provider;
    case Roles::CommunicationWithRole:
        return _calls[row].communicationWith;
    case Roles::DirectionRole:
        return QVariant::fromValue(_calls[row].direction);
    case Roles::StateRole:
        return QVariant::fromValue(_calls[row].state);
    case Roles::StateReasonRole:
        return QVariant::fromValue(_calls[row].stateReason);
    case Roles::CallAttemptDurationRole:
        return _calls[row].callAttemptDuration;
    case Roles::StartedAtRole:
        return _calls[row].startedAt;
    case Roles::DurationRole:
        return _calls[row].duration;
    }
    return {};
}

int ActiveCallModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _calls.size();
}

void ActiveCallModel::onUtilsCallAdded(const QString &deviceUni,
                                       const QString &callUni,
                                       const DialerTypes::CallDirection &callDirection,
                                       const DialerTypes::CallState &callState,
                                       const DialerTypes::CallStateReason &callStateReason,
                                       const QString communicationWith)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    _callUtils->fetchCalls();
    setCommunicationWith(communicationWith);
    _callsTimer.start();
}

void ActiveCallModel::onUtilsCallDeleted(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    _callUtils->fetchCalls();
    _callsTimer.stop();
}

void ActiveCallModel::onUtilsCallStateChanged(const DialerTypes::CallData &callData)
{
    qDebug() << Q_FUNC_INFO << callData.state << callData.stateReason;
    auto callState = callData.state;

    if (callState == DialerTypes::CallState::Active) {
        _callsTimer.start();
    }
    if (callState == DialerTypes::CallState::RingingIn) {
        _callsTimer.start();
    }
    if (callState == DialerTypes::CallState::Terminated) {
        _callsTimer.stop();
    }

    // find call by id and update all the stuff including the duration
    for (int i = 0; i < _calls.size(); i++) {
        auto call = _calls.at(i);
        if (call.id == callData.id) {
            call = callData;
            Q_EMIT dataChanged(index(i), index(i));
            return;
        }
    }
}

void ActiveCallModel::onUtilsCallsChanged(const DialerTypes::CallDataVector &fetchedCalls)
{
    qDebug() << Q_FUNC_INFO << _calls.size() << fetchedCalls.size();
    beginResetModel();
    _calls = fetchedCalls;
    endResetModel();
    bool active = (_calls.size() > 0);
    setActive(active);
    if (!active) {
        return;
    }
    bool incoming = false;
    for (int i = 0; i < _calls.size(); i++) {
        const auto call = _calls.at(i);
        // trying to determine current active call
        // should be checked could it be improved
        // with with DialerTypes::CallDirection
        if ((call.state != DialerTypes::CallState::Unknown) && (call.state != DialerTypes::CallState::Held) && (call.state != DialerTypes::CallState::Waiting)
            && (call.state != DialerTypes::CallState::Terminated)) {
            setCommunicationWith(call.communicationWith);
            setDuration(call.duration);
        }
        if (call.direction == DialerTypes::CallDirection::Incoming) {
            if (call.state == DialerTypes::CallState::RingingIn) {
                incoming = true;
                break;
            }
        }
    }
    setIncoming(incoming);
}

bool ActiveCallModel::active() const
{
    return _active;
}

void ActiveCallModel::setActive(bool newActive)
{
    if (_active == newActive)
        return;
    _active = newActive;
    qDebug() << Q_FUNC_INFO;
    Q_EMIT activeChanged();
}

bool ActiveCallModel::incoming() const
{
    return _incoming;
}

void ActiveCallModel::setIncoming(bool newIncoming)
{
    if (_incoming == newIncoming)
        return;
    _incoming = newIncoming;
    Q_EMIT incomingChanged();
}

QString ActiveCallModel::communicationWith() const
{
    return _communicationWith;
}

void ActiveCallModel::setCommunicationWith(const QString communicationWith)
{
    if (_communicationWith == communicationWith)
        return;
    _communicationWith = communicationWith;
    Q_EMIT communicationWithChanged();
}

qulonglong ActiveCallModel::duration() const
{
    return _duration;
}

void ActiveCallModel::setDuration(qulonglong duration)
{
    if (_duration == duration)
        return;
    _duration = duration;
    Q_EMIT durationChanged();
}

void ActiveCallModel::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    if (!callUtils->isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallUtils interface";
        return;
    }
    _callUtils = callUtils;

    connect(_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &ActiveCallModel::onUtilsCallStateChanged);
    connect(_callUtils, &org::kde::telephony::CallUtils::callAdded, this, &ActiveCallModel::onUtilsCallAdded);
    connect(_callUtils, &org::kde::telephony::CallUtils::callDeleted, this, &ActiveCallModel::onUtilsCallDeleted);
    connect(_callUtils, &org::kde::telephony::CallUtils::callsChanged, this, &ActiveCallModel::onUtilsCallsChanged);

    _callsTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    connect(&_callsTimer, &QTimer::timeout, this, [this]() {
        // minimize the number of method calls by incrementing the duration on the client side too
        // see also (D-Bus API Design Guidelines):
        // https://dbus.freedesktop.org/doc/dbus-api-design.html
        _updateTimers();
    });

    callUtils->fetchCalls(); // TODO: simplify sync
}

void ActiveCallModel::_updateTimers()
{
    for (int i = 0; i < _calls.size(); i++) {
        auto call = _calls.at(i);
        auto callState = call.state;

        if (callState == DialerTypes::CallState::RingingIn) {
            qDebug() << "incoming call";
            call.callAttemptDuration++;
            Q_EMIT dataChanged(index(i), index(i), {CallAttemptDurationRole});
        }
        if (callState == DialerTypes::CallState::Active) {
            qDebug() << "call started";
            call.duration++;
            setDuration(call.duration);
            Q_EMIT dataChanged(index(i), index(i), {DurationRole});
        }
    }
}
