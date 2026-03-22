/*
 *   SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QImage>
#include <QObject>
#include <QQmlEngine>
#include <QQuickWindow>

#include <KContacts/Addressee>
#include <KContacts/PhoneNumber>

#include <KSharedConfig>
#include <memory>

class QFileDialog;

class ContactController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    ContactController();

    Q_INVOKABLE QByteArray addresseeToVCard(const KContacts::Addressee &addressee);
    Q_INVOKABLE KContacts::Addressee emptyAddressee();
    Q_INVOKABLE KContacts::PhoneNumber createPhoneNumber(const QString &number);

private:
    KConfig m_dataResource;
};
