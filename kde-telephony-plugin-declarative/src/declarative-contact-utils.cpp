// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-contact-utils.h"

#include <KContacts/VCardConverter>
#include <KPeopleBackend/AbstractContact>
#include <KPeople/PersonData>

#include "callutilsinterface.h"

template<typename T>
static QVariantList toVariantList(const QVector<T> &v)
{
    QVariantList l;
    l.reserve(v.size());
    std::transform(v.begin(), v.end(), std::back_inserter(l), [](const T &elem) {
        return QVariant::fromValue(elem);
    });
    return l;
}

const static KContacts::VCardConverter converter;

static QSharedPointer<KPeople::PersonData> contactData(const QString &uri)
{
    return QSharedPointer<KPeople::PersonData>(new KPeople::PersonData(uri));
}

DeclarativeContactUtils::DeclarativeContactUtils(QObject *parent)
    : org::kde::telephony::ContactUtils(QString::fromLatin1(staticInterfaceName()),
                                        QStringLiteral("/org/kde/telephony/ContactUtils/tel/mm"),
                                        QDBusConnection::sessionBus(),
                                        parent)
{
    if (!isValid()) {
        qDebug() << Q_FUNC_INFO << "Could not initiate ContactUtils interface";
        return;
    }
}

QString DeclarativeContactUtils::displayString(const QString &contact)
{
    QString result = contact;
    QDBusPendingReply<QString> reply = org::kde::telephony::ContactUtils::displayString(contact);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
    } else {
        result = reply;
    }
    return result;
}

QVariantList DeclarativeContactUtils::phoneNumbers(const QString &kPeopleUri)
{
    auto person = contactData(kPeopleUri);
    auto vcard = person->contactCustomProperty(KPeople::AbstractContact::VCardProperty).toByteArray();
    auto addressee = converter.parseVCard(vcard);

    return toVariantList(addressee.phoneNumbers());
}
