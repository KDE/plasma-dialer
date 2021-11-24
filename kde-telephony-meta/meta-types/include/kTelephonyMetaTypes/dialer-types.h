// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#pragma once

#include <type_traits>

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDateTime>
#include <QObject>
#include <QString>

namespace DialerTypes
{
Q_NAMESPACE

enum class CallState { Unknown, Dialing, RingingOut, RingingIn, Active, Held, Waiting, Terminated };

enum class CallStateReason {
    Unknown,
    OutgoingStarted,
    IncomingNew,
    Accepted,
    TerminatedReason,
    RefusedOrBusy,
    Error,
    AudioSetupFailed,
    Transferred,
    Deflected
};

enum class CallDirection {
    Unknown,
    Incoming,
    Outgoing,
};

Q_ENUM_NS(CallState)
Q_ENUM_NS(CallStateReason)
Q_ENUM_NS(CallDirection)

struct CallData {
    QString id; // event id

    QString protocol; // protocol id: tel, mtx, tg
    QString provider; // provider id: sim value displayed with sim name/icon or homeserver id)
    QString account; // account id: phone number or mtx id)

    QString communicationWith; // interlocutor: a phone number or room id
    DialerTypes::CallDirection direction; // direction (incoming/outgoing/unknown)
    DialerTypes::CallState state; // call state
    DialerTypes::CallStateReason stateReason; // call state reason

    int callAttemptDuration; // call attempt duration
    QDateTime startedAt; // started at
    int duration; // call duration
};

// Marshalling for CallState, CallStateReason and CallDirection enums:

template<typename T, typename TEnum>
struct QDBusEnumMarshal;

template<typename T>
struct QDBusEnumMarshal<T, std::true_type> {
    static QDBusArgument &marshal(QDBusArgument &argument, const T &source)
    {
        argument.beginStructure();
        argument << static_cast<int>(source);
        argument.endStructure();
        return argument;
    }

    static const QDBusArgument &unmarshal(const QDBusArgument &argument, T &source)
    {
        int a;
        argument.beginStructure();
        argument >> a;
        argument.endStructure();

        source = static_cast<T>(a);
        return argument;
    }
};

// Marshalling for CallData struct:

template<typename T>
struct QDBusCallDataMarshal;

template<>
struct QDBusCallDataMarshal<CallData> {
    static QDBusArgument &marshal(QDBusArgument &argument, const CallData &source)
    {
        QVariantMap dict;
        dict.insert(QStringLiteral("id"), source.id);
        dict.insert(QStringLiteral("protocol"), source.protocol);
        dict.insert(QStringLiteral("account"), source.account);
        dict.insert(QStringLiteral("provider"), source.provider);
        dict.insert(QStringLiteral("communicationWith"), source.communicationWith);
        dict.insert(QStringLiteral("direction"), static_cast<int>(source.direction));
        dict.insert(QStringLiteral("state"), static_cast<int>(source.state));
        dict.insert(QStringLiteral("stateReason"), static_cast<int>(source.stateReason));
        dict.insert(QStringLiteral("callAttemptDuration"), source.callAttemptDuration);
        dict.insert(QStringLiteral("startedAt"), source.startedAt.toSecsSinceEpoch());
        dict.insert(QStringLiteral("duration"), source.duration);

        argument.beginStructure();
        argument << dict;
        argument.endStructure();
        return argument;
    };

    static const QDBusArgument &unmarshal(const QDBusArgument &argument, CallData &source)
    {
        QVariantMap dict;
        argument.beginStructure();
        argument >> dict;
        argument.endStructure();

        source.id = dict.value(QStringLiteral("id")).toString();
        source.protocol = dict.value(QStringLiteral("protocol")).toString();
        source.account = dict.value(QStringLiteral("account")).toString();
        source.provider = dict.value(QStringLiteral("provider")).toString();
        source.communicationWith = dict.value(QStringLiteral("communicationWith")).toString();

        source.direction = dict.value(QStringLiteral("direction"), 0).value<DialerTypes::CallDirection>();
        source.state = dict.value(QStringLiteral("state"), 0).value<DialerTypes::CallState>();
        source.stateReason = dict.value(QStringLiteral("stateReason"), 0).value<DialerTypes::CallStateReason>();

        source.callAttemptDuration = dict.value(QStringLiteral("callAttemptDuration"), 0).toInt();
        source.startedAt = QDateTime::fromSecsSinceEpoch((uint)dict.value(QStringLiteral("startedAt"), 0).toLongLong());
        source.duration = dict.value(QStringLiteral("duration"), 0).toInt();

        return argument;
    };
};

template<typename T>
QDBusArgument &operator<<(QDBusArgument &argument, const T &source)
{
    if constexpr (std::is_same_v<T, CallData>) {
        return QDBusCallDataMarshal<CallData>::marshal(argument, source);
    } else {
        return QDBusEnumMarshal<T, typename std::is_enum<T>::type>::marshal(argument, source);
    }
}

template<typename T>
const QDBusArgument &operator>>(const QDBusArgument &argument, T &source)
{
    if constexpr (std::is_same_v<T, CallData>) {
        return QDBusCallDataMarshal<CallData>::unmarshal(argument, source);
    } else {
        return QDBusEnumMarshal<T, typename std::is_enum<T>::type>::unmarshal(argument, source);
    };
}

using CallDataVector = QVector<DialerTypes::CallData>;

} // namespace DialerTypes

Q_DECLARE_METATYPE(DialerTypes::CallState)
Q_DECLARE_METATYPE(DialerTypes::CallStateReason)
Q_DECLARE_METATYPE(DialerTypes::CallDirection)
Q_DECLARE_METATYPE(DialerTypes::CallData)
Q_DECLARE_METATYPE(DialerTypes::CallDataVector)

namespace DialerTypes
{
inline void registerMetaTypes()
{
    qRegisterMetaType<DialerTypes::CallState>("DialerTypesCallState");
    qDBusRegisterMetaType<DialerTypes::CallState>();

    qRegisterMetaType<DialerTypes::CallStateReason>("DialerTypesCallStateReason");
    qDBusRegisterMetaType<DialerTypes::CallStateReason>();

    qRegisterMetaType<DialerTypes::CallDirection>("DialerTypesCallDirection");
    qDBusRegisterMetaType<DialerTypes::CallDirection>();

    qRegisterMetaType<DialerTypes::CallData>("DialerTypesCallData");
    qDBusRegisterMetaType<DialerTypes::CallData>();

    qRegisterMetaType<DialerTypes::CallDataVector>("DialerTypesCallDataVector");
    qDBusRegisterMetaType<DialerTypes::CallDataVector>();
}
}
