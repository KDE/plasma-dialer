/*
 * Copyright 2015 Marco Martin <mart@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.

 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**/

#include "dialerutils.h"

#include <QDebug>

#include <TelepathyQt/Constants>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/PendingContacts>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/Types>
#include <qofono-qt5/qofonomanager.h>
#include <qofono-qt5/qofonomessagewaiting.h>

#include <KPeople/PersonData>

#include "phonenumbers/asyoutypeformatter.h"
#include "phonenumbers/phonenumberutil.h"

#include "contactmapper.h"

DialerUtils::DialerUtils(const Tp::AccountPtr &simAccount, QObject *parent)
    : QObject(parent)
    , m_simAccount(simAccount)
{
    if (!m_simAccount) {
        return;
    }

    Tp::PendingReady *op = m_simAccount->becomeReady(Tp::Features() << Tp::Account::FeatureCore);

    connect(op, &Tp::PendingOperation::finished, [=]() {
        if (op->isError()) {
            qWarning() << "SIM card account failed to get ready:" << op->errorMessage();
        } else {
            qDebug() << "SIM Account ready to use";
        }
    });

    QOfonoManager manager;
    manager.getModems();
    QString modemPath = manager.defaultModem();
    if (!modemPath.isEmpty()) {
        m_msgWaiting = new QOfonoMessageWaiting(this);
        m_msgWaiting->setModemPath(modemPath);
    }
}

DialerUtils::~DialerUtils() = default;

void DialerUtils::dial(const QString &number)
{
    // FIXME: this should be replaced by kpeople thing
    qDebug() << "Starting call...";
    if (m_simAccount) {
        Tp::PendingChannelRequest *pendingChannel = m_simAccount->ensureAudioCall(number);
        connect(pendingChannel, &Tp::PendingChannelRequest::finished, pendingChannel, [=]() {
            if (pendingChannel->isError()) {
                qWarning() << "Error when requesting channel" << pendingChannel->errorMessage();
                setCallState(CallState::Failed);
            }
        });
    } else {
        setCallState(CallState::Failed);
    }
}

DialerUtils::CallState DialerUtils::callState() const
{
    return m_callState;
}

QString DialerUtils::formatNumber(const QString &number)
{
    using namespace ::i18n::phonenumbers;

    // Get formatter instance
    QLocale locale;
    QStringList qcountry = locale.name().split('_');
    const QString &countrycode(qcountry.constLast());
    string country = countrycode.toUtf8().constData();
    PhoneNumberUtil *util = PhoneNumberUtil::GetInstance();
    AsYouTypeFormatter *formatter = util->PhoneNumberUtil::GetAsYouTypeFormatter(country);

    // Normalize input
    string stdnumber = number.toUtf8().constData();
    util->NormalizeDiallableCharsOnly(&stdnumber);

    // Format
    string formatted;
    formatter->Clear();
    for (char &c : stdnumber) {
        formatter->InputDigit(c, &formatted);
    }
    delete formatter;

    return QString::fromStdString(formatted);
}

void DialerUtils::setCallState(const CallState state)
{
    if (m_callState != state) {
        m_callState = state;
        Q_EMIT callStateChanged();
    }
}

uint DialerUtils::callDuration() const
{
    return m_callDuration;
}

void DialerUtils::setCallDuration(uint duration)
{
    m_callDuration = duration;
    Q_EMIT callDurationChanged();
}

QString DialerUtils::callContactAlias() const
{
    return m_callContactAlias;
}

void DialerUtils::setCallContactAlias(const QString &contactAlias)
{
    if (m_callContactAlias != contactAlias) {
        m_callContactAlias = contactAlias;
        Q_EMIT callContactAliasChanged();
    }
}

QString DialerUtils::callContactNumber() const
{
    return m_callContactNumber;
}

void DialerUtils::setCallContactNumber(const QString &contactNumber)
{
    if (m_callContactNumber != contactNumber) {
        m_callContactNumber = contactNumber;
        Q_EMIT callContactNumberChanged();
    }
}

bool DialerUtils::isIncomingCall() const
{
    return m_isIncomingCall;
}

void DialerUtils::setIsIncomingCall(bool isIncomingCall)
{
    if (m_isIncomingCall != isIncomingCall) {
        m_isIncomingCall = isIncomingCall;
        Q_EMIT isIncomingCallChanged();
    }
}

void DialerUtils::emitCallEnded()
{
    qDebug() << "Call ended:" << m_callContactNumber << m_callDuration;
    Q_EMIT callEnded(m_callContactNumber, m_callDuration, m_isIncomingCall);
    m_callDuration = 0;
    m_callContactNumber = QString();
    m_callContactAlias = QString();
}

void DialerUtils::resetMissedCalls()
{
    m_missedCalls = 0;
}

void DialerUtils::getImeis()
{
    // based on https://git.sailfishos.org/jpetrell/ssu/commit/1b2a59378713dd93b8b215a99f7f2aeb524b35bd
    QStringList imeis;

    QDBusMessage reply = QDBusConnection::systemBus().call(QDBusMessage::createMethodCall("org.ofono", "/", "org.ofono.Manager", "GetModems"));

    for (const QVariant &v : reply.arguments()) {
        if (v.canConvert<QDBusArgument>()) {
            const QDBusArgument arg = v.value<QDBusArgument>();
            if (arg.currentType() == QDBusArgument::ArrayType) {
                arg.beginArray();
                while (!arg.atEnd()) {
                    if (arg.currentType() == QDBusArgument::StructureType) {
                        QString path;
                        QVariantMap props;

                        arg.beginStructure();
                        arg >> path >> props;
                        arg.endStructure();

                        if (props.contains("Serial")) {
                            imeis << props["Serial"].toString();
                        }
                    }
                }
                arg.endArray();
            }
        }
    }
    Q_EMIT displayImeis(imeis);
}

QString DialerUtils::getVoicemailNumber()
{
    if (!m_voicemailNumber.isEmpty())
        return m_voicemailNumber;
    if (m_msgWaiting == nullptr)
        return QString();
    QString number = m_msgWaiting->voicemailMailboxNumber();
    m_voicemailNumber = number;
    return number;
}

QString DialerUtils::callContactDisplayString() const
{
    const QString uri = ContactMapper::instance().uriForNumber(m_callContactAlias);

    KPeople::PersonData person(uri);

    const QString name = person.name();

    if (!name.isEmpty()) {
        return name;
    }

    return m_callContactAlias;
}

#include "moc_dialerutils.cpp"
