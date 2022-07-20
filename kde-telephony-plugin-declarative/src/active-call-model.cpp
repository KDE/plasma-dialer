// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "active-call-model.h"

constexpr int CALL_DURATION_UPDATE_DELAY = 1000;

ActiveCallModel::ActiveCallModel(QObject *parent)
    : CallModel(parent)
{
    _callUtils = new DeclarativeCallUtils(this);

    if (!_callUtils->isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallUtils ModemManager interface";
        return;
    }

    connect(_callUtils, &DeclarativeCallUtils::callStateChanged, this, &ActiveCallModel::onCallStateChanged);
    connect(_callUtils, &DeclarativeCallUtils::callAdded, this, &ActiveCallModel::onCallAdded);
    connect(_callUtils, &DeclarativeCallUtils::callDeleted, this, &ActiveCallModel::onCallDeleted);
    connect(_callUtils, &DeclarativeCallUtils::fetchedCallsChanged, this, &ActiveCallModel::onFetchedCallsChanged);
    connect(_callUtils, &DeclarativeCallUtils::callDurationChanged, this, &ActiveCallModel::onCallDurationChanged);

    _callsTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    connect(&_callsTimer, &QTimer::timeout, this, [this]() {
        _callUtils->fetchCalls();
        Q_EMIT dataChanged(index(0), index(rowCount()), {DurationRole, CallAttemptDurationRole});
    });

    _callUtils->fetchCalls();
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
    for (int i = 0; i <= _calls.size(); i++) {
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

void ActiveCallModel::onCallAdded(const QString &deviceUni,
                                  const QString &callUni,
                                  const DialerTypes::CallDirection &callDirection,
                                  const DialerTypes::CallState &callState,
                                  const DialerTypes::CallStateReason &callStateReason,
                                  const QString communicationWith)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    _callUtils->fetchCalls();
}

void ActiveCallModel::onCallDeleted(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    _callUtils->fetchCalls();
}

void ActiveCallModel::onCallStateChanged(const QString &deviceUni,
                                         const QString &callUni,
                                         const DialerTypes::CallDirection &callDirection,
                                         const DialerTypes::CallState &callState,
                                         const DialerTypes::CallStateReason &callStateReason)
{
    qDebug() << Q_FUNC_INFO << deviceUni << callUni << callDirection << callState << callStateReason;
    _callUtils->fetchCalls();

    if (callState == DialerTypes::CallState::Terminated) {
        _callDuration = 0;
        Q_EMIT callDurationChanged();
    }

    setCallState(callState);
}

void ActiveCallModel::onFetchedCallsChanged(const DialerTypes::CallDataVector &fetchedCalls)
{
    qDebug() << Q_FUNC_INFO << _calls.size() << fetchedCalls.size();
    beginResetModel();
    _calls = fetchedCalls;
    endResetModel();
    bool active = (_calls.size() > 0);
    setActive(active);
}

void ActiveCallModel::onCallDurationChanged(const int duration)
{
    _callDuration = duration;
    Q_EMIT callDurationChanged();
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

DialerTypes::CallState ActiveCallModel::callState() const
{
    return _callState;
}

void ActiveCallModel::setCallState(const DialerTypes::CallState state)
{
    if (_callState == state)
        return;
    _callState = state;
    qDebug() << Q_FUNC_INFO;
    Q_EMIT callStateChanged();
}

uint ActiveCallModel::callDuration() const
{
    return _callDuration;
}

void ActiveCallModel::setCallDuration(uint duration)
{
    _callDuration = duration;
    qDebug() << Q_FUNC_INFO;
    Q_EMIT callDurationChanged();
}
