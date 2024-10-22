/*
 *   SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami
import org.kde.people as KPeople
import org.kde.kirigamiaddons.components as Components
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.telephony
import "call"

Kirigami.ScrollablePage {
    id: contactsPage

    // page animation
    property real yTranslate: 0

    title: i18n("Contacts")
    icon.name: "view-pim-contacts"
    actions: [
        Kirigami.Action {
            icon.name: "settings-configure"
            displayHint: Kirigami.DisplayHint.IconOnly
            visible: !applicationWindow().isWidescreen
            enabled: !applicationWindow().lockscreenMode
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    Component {
        id: callPopup

        PhoneNumberDialog {
            parent: contactsPage.T.Overlay.overlay
        }

    }

    ListView {
        id: contactsList

        section.property: "display"
        section.criteria: ViewSection.FirstCharacter
        clip: true
        reuseItems: true
        boundsBehavior: Flickable.StopAtBounds

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No contacts have a phone number set")
            icon.name: "contact-new-symbolic"
            visible: contactsList.count === 0
        }

        transform: Translate {
            y: yTranslate
        }

        section.delegate: Kirigami.ListSectionHeader {
            text: section
        }

        model: KPeople.PersonsSortFilterProxyModel {
            id: contactsProxyModel

            requiredProperties: "phoneNumber"
            filterRole: Qt.DisplayRole
            sortRole: Qt.DisplayRole
            filterCaseSensitivity: Qt.CaseInsensitive
            Component.onCompleted: sort(0)

            sourceModel: KPeople.PersonsModel {
                id: contactsModel
            }

        }

        delegate: Delegates.RoundedItemDelegate {
            id: delegateItem

            width: contactsList.width
            implicitHeight: Kirigami.Units.iconSizes.medium + Kirigami.Units.largeSpacing * 2
            verticalPadding: 0
            onReleased: {
                const phoneNumbers = ContactUtils.phoneNumbers(model.personUri);
                if (phoneNumbers.length === 1) {
                    applicationWindow().call(phoneNumbers[0].normalizedNumber);
                } else {
                    const pop = callPopup.createObject(this, {
                        "numbers": phoneNumbers,
                        "title": i18n("Select number to call")
                    });
                    pop.onNumberSelected.connect((number) => {
                        return applicationWindow().call(number);
                    });
                    pop.open();
                }
            }

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                Components.Avatar {
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    source: model.photoImageProviderUri
                    name: model.display
                    imageMode: Components.Avatar.ImageMode.AdaptiveImageOrInitals
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    Controls.Label {
                        id: labelItem

                        Layout.fillWidth: true
                        Layout.alignment: subtitleItem.visible ? Qt.AlignLeft | Qt.AlignBottom : Qt.AlignLeft | Qt.AlignVCenter
                        text: model && model.display
                        elide: Text.ElideRight
                        color: Kirigami.Theme.textColor
                    }

                    Controls.Label {
                        id: subtitleItem

                        visible: text
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        text: model.phoneNumber
                        elide: Text.ElideRight
                        color: Kirigami.Theme.textColor
                        opacity: 0.7
                        font: Kirigami.Theme.smallFont
                    }

                }

            }

        }

    }

    header: ColumnLayout {
        spacing: 0

        Kirigami.InlineMessage {
            id: daemonsError

            type: Kirigami.MessageType.Error
            text: i18n("Telephony daemons are not responding")
            visible: !ContactUtils.isValid
            position: Kirigami.InlineMessage.Header
            Layout.fillWidth: true
        }

        InCallInlineMessage {
            position: Kirigami.InlineMessage.Header
            Layout.fillWidth: true
        }

        Kirigami.SearchField {
            id: searchField

            onTextChanged: contactsProxyModel.setFilterFixedString(text)
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
        }

    }

}
