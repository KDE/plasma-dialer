// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

#include "declarative-contact-utils.h"

#include <KContacts/VCardConverter>
#include <KPeopleBackend/AbstractContact>
#include <KPeople/PersonData>

#include <QRegularExpression>

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
    return toVariantList(internalPhoneNumbers(kPeopleUri));
}

QString DeclarativeContactUtils::phoneNumberToContactUri(const QString &phoneNumber)
{
    KPeople::PersonsModel *personsModel = new KPeople::PersonsModel{this};
    personsModel->deleteLater();

    for (int i = 0; i < personsModel->rowCount(); ++i) {
        QModelIndex index = personsModel->index(i, 0, {});
        if (index.isValid()) {
            QString uri = personsModel->data(index, KPeople::PersonsModel::PersonUriRole).toString();
            auto list = internalPhoneNumbers(uri);

            for (auto number : list) {
                if (numericOnlyString(number.number()) == numericOnlyString(phoneNumber)) {
                    return uri;
                }
            }
        }
    }
    return {};
}

QString DeclarativeContactUtils::photoImageProviderUri(const QString &contactUri)
{
    return u"image://kpeople-avatar/" % QString::fromUtf8(contactUri.toUtf8().toBase64()) % u"#" % QString::number(QRandomGenerator::system()->generate());
}

bool DeclarativeContactUtils::isValid() const
{
    return org::kde::telephony::ContactUtils::isValid();
}

QList<KContacts::PhoneNumber> DeclarativeContactUtils::internalPhoneNumbers(const QString &kPeopleUri) const
{
    auto person = contactData(kPeopleUri);
    auto vcard = person->contactCustomProperty(KPeople::AbstractContact::VCardProperty).toByteArray();
    auto addressee = converter.parseVCard(vcard);
    return addressee.phoneNumbers();
}

QString DeclarativeContactUtils::numericOnlyString(QString str) const
{
    QRegularExpression nonNumericRegex(QStringLiteral("\\D"));
    return str.remove(nonNumericRegex);
}
