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
    if (!m_callUtils) {
        qDebug() << Q_FUNC_INFO << "CallUtils is not initiated";
        return;
    }
    QString deviceUni;
    QString callUni;
    m_callUtils->sendDtmf(deviceUni, callUni, tones);
}

void ActiveCallModel::dial(const QString &deviceUni, const QString &number)
{
    if (!m_callUtils) {
        qDebug() << Q_FUNC_INFO << "CallUtils is not initiated";
        return;
    }
    m_callUtils->dial(deviceUni, number);
}

QString ActiveCallModel::activeCallUni()
{
    QString activeCallUni;
    if (m_calls.size() < 1) {
        qDebug() << Q_FUNC_INFO << "empty active calls list";
        return activeCallUni;
    }
    for (int i = 0; i < m_calls.size(); i++) {
        const auto call = m_calls.at(i);
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
        return m_calls[row].id;
    case Roles::ProtocolRole:
        return m_calls[row].protocol;
    case Roles::AccountRole:
        return m_calls[row].account;
    case Roles::ProviderRole:
        return m_calls[row].provider;
    case Roles::CommunicationWithRole:
        return m_calls[row].communicationWith;
    case Roles::DirectionRole:
        return QVariant::fromValue(m_calls[row].direction);
    case Roles::StateRole:
        return QVariant::fromValue(m_calls[row].state);
    case Roles::StateReasonRole:
        return QVariant::fromValue(m_calls[row].stateReason);
    case Roles::CallAttemptDurationRole:
        return m_calls[row].callAttemptDuration;
    case Roles::StartedAtRole:
        return m_calls[row].startedAt;
    case Roles::DurationRole:
        return m_calls[row].duration;
    }
    return {};
}

int ActiveCallModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_calls.size();
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
    Q_UNUSED(callDirection);
    Q_UNUSED(callState);
    Q_UNUSED(callStateReason);
    if (!m_callUtils) {
        qDebug() << Q_FUNC_INFO << "CallUtils is not initiated";
        return;
    }
    m_callUtils->fetchCalls();
    setCommunicationWith(communicationWith);
    m_callsTimer.start();
}

void ActiveCallModel::onUtilsCallDeleted(const QString &deviceUni, const QString &callUni)
{
    Q_UNUSED(deviceUni);
    Q_UNUSED(callUni);
    if (!m_callUtils) {
        qDebug() << Q_FUNC_INFO << "CallUtils is not initiated";
        return;
    }
    m_callUtils->fetchCalls();
    m_callsTimer.stop();
    setDuration(0);
}

void ActiveCallModel::onUtilsCallStateChanged(const DialerTypes::CallData &callData)
{
    qDebug() << Q_FUNC_INFO << callData.state << callData.stateReason;
    DialerTypes::CallState callState = callData.state;

    if (callState == DialerTypes::CallState::Active) {
        m_callsTimer.start();
    }
    if (callState == DialerTypes::CallState::RingingIn) {
        m_callsTimer.start();
    }
    if (callState == DialerTypes::CallState::Terminated) {
        m_callsTimer.stop();
    }

    if (m_calls.size() < 1) {
        qDebug() << Q_FUNC_INFO << "empty active calls list";
    }
    // find call by id and update all the stuff including the duration
    for (int i = 0; i < m_calls.size(); i++) {
        if (m_calls[i].id == callData.id) {
            m_calls[i] = callData;
            Q_EMIT dataChanged(index(i), index(i));
            return;
        }
    }
}

void ActiveCallModel::onUtilsCallsChanged(const DialerTypes::CallDataVector &fetchedCalls)
{
    qDebug() << Q_FUNC_INFO << m_calls.size() << fetchedCalls.size();
    beginResetModel();
    m_calls = fetchedCalls;
    endResetModel();
    bool active = (m_calls.size() > 0);
    setActive(active);
    if (!active) {
        return;
    }
    m_callsTimer.start();
    bool incoming = false;
    for (int i = 0; i < m_calls.size(); i++) {
        const auto call = m_calls.at(i);
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
    return m_active;
}

void ActiveCallModel::setActive(bool newActive)
{
    if (m_active == newActive)
        return;
    m_active = newActive;
    qDebug() << Q_FUNC_INFO;
    Q_EMIT activeChanged();
}

bool ActiveCallModel::incoming() const
{
    return m_incoming;
}

void ActiveCallModel::setIncoming(bool newIncoming)
{
    if (m_incoming == newIncoming)
        return;
    m_incoming = newIncoming;
    Q_EMIT incomingChanged();
}

QString ActiveCallModel::communicationWith() const
{
    return m_communicationWith;
}

void ActiveCallModel::setCommunicationWith(const QString communicationWith)
{
    if (m_communicationWith == communicationWith)
        return;
    m_communicationWith = communicationWith;
    Q_EMIT communicationWithChanged();
}

qulonglong ActiveCallModel::duration() const
{
    return m_duration;
}

void ActiveCallModel::setDuration(qulonglong duration)
{
    if (m_duration == duration)
        return;
    m_duration = duration;
    Q_EMIT durationChanged();
}

void ActiveCallModel::setCallUtils(org::kde::telephony::CallUtils *callUtils)
{
    if (!callUtils) {
        qDebug() << Q_FUNC_INFO << "Could not initiate CallUtils interface";
        return;
    }
    m_callUtils = callUtils;

    connect(m_callUtils, &org::kde::telephony::CallUtils::callStateChanged, this, &ActiveCallModel::onUtilsCallStateChanged);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callAdded, this, &ActiveCallModel::onUtilsCallAdded);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callDeleted, this, &ActiveCallModel::onUtilsCallDeleted);
    connect(m_callUtils, &org::kde::telephony::CallUtils::callsChanged, this, &ActiveCallModel::onUtilsCallsChanged);

    m_callsTimer.setInterval(CALL_DURATION_UPDATE_DELAY);
    connect(&m_callsTimer, &QTimer::timeout, this, [this]() {
        // minimize the number of method calls by incrementing the duration on the client side too
        // see also (D-Bus API Design Guidelines):
        // https://dbus.freedesktop.org/doc/dbus-api-design.html
        updateTimers();
    });

    callUtils->fetchCalls(); // TODO: simplify sync
}

void ActiveCallModel::updateTimers()
{
    for (int i = 0; i < m_calls.size(); i++) {
        DialerTypes::CallData call = m_calls.at(i);
        DialerTypes::CallState callState = call.state;

        if (callState == DialerTypes::CallState::RingingIn) {
            qDebug() << "incoming call";
            call.callAttemptDuration++;
            Q_EMIT dataChanged(index(i), index(i), {CallAttemptDurationRole});
        }
        if (callState == DialerTypes::CallState::Active) {
            qDebug() << "call started";
            call.duration++;
            m_calls[i].duration = call.duration;
            setDuration(call.duration);
            Q_EMIT dataChanged(index(i), index(i), {DurationRole});
        }
    }
}
