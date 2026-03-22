/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Templates as T

import Qt.labs.platform
import Qt5Compat.GraphicalEffects

import org.kde.people as KPeople
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.contacts as KContacts

import org.kde.plasma.dialer

FormCard.FormCardPage {
    id: root
    title: i18n("Adding contact")
    property QtObject person
    property KContacts.addressee addressee: ContactController.emptyAddressee()
    property list<KContacts.phoneNumber> pendingPhoneNumbers: addressee.phoneNumbers

    signal save()

    property string phoneNumber: ""

    enabled: !person

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true

        FormCard.FormTextFieldDelegate{
            id: name
            label: i18n("Name")

            text: root.addressee.formattedName
            onAccepted: {
                root.addressee.formattedName = text
            }
            Connections {
                target: root
                function onSave() {
                    name.accepted()
                }
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextFieldDelegate{
                id: phoneField

                label: i18n("Phone")
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: root.phoneNumber
                onAccepted: {
                    root.pendingPhoneNumbers.number = text
                }

                Connections {
                    target: root
                    function onSave() {
                        var numbers = root.pendingPhoneNumbers
                        numbers.push(ContactController.createPhoneNumber(phoneField.text))
                        pendingPhoneNumbers = numbers
                        phoneField.accepted()
                        root.addressee.phoneNumbers = root.pendingPhoneNumbers
                    }
                }
            }
    }

    footer: T.Control {
        id: footerToolBar

        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                implicitContentWidth + leftPadding + rightPadding)
        implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                implicitContentHeight + topPadding + bottomPadding)

        leftPadding: Kirigami.Units.smallSpacing
        rightPadding: Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.smallSpacing
        topPadding: Kirigami.Units.smallSpacing // + footerSeparator.implicitHeight

        contentItem: RowLayout {
            spacing: parent.spacing

            // footer buttons
            Controls.DialogButtonBox {
                // we don't explicitly set padding, to let the style choose the padding
                id: dialogButtonBox
                standardButtons: Controls.DialogButtonBox.Save

                Layout.fillWidth: true
                Layout.alignment: dialogButtonBox.alignment

                position: Controls.DialogButtonBox.Footer

                onAccepted: {
                    root.save();
                    if (!KPeople.PersonPluginManager.addContact({ "vcard": ContactController.addresseeToVCard(addressee) }))
                                console.warn("could not create contact")

                    Kirigami.PageStack.pop()
                }
            }
        }
    }
}
