/*
 *   SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.people as KPeople

import org.kde.telephony

import "call"

Kirigami.ScrollablePage {
    id: contactsPage
    title: i18n("Contacts")
    icon.name: "view-pim-contacts"

    // page animation
    property real yTranslate: 0
    
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

        PhoneNumberDialog {}
    }
    
    header: ColumnLayout {
        anchors.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.smallSpacing
        
        InCallInlineMessage {}
        
        Kirigami.SearchField {
            id: searchField
            onTextChanged: contactsProxyModel.setFilterFixedString(text)
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
        }
    }

    ListView {
        id: contactsList
        transform: Translate { y: yTranslate }

        section.property: "display"
        section.criteria: ViewSection.FirstCharacter
        section.delegate: Kirigami.ListSectionHeader {
            text: section
        }
        clip: true
        reuseItems: true

        model: KPeople.PersonsSortFilterProxyModel {
            id: contactsProxyModel
            sourceModel: KPeople.PersonsModel {
                id: contactsModel
            }
            requiredProperties: "phoneNumber"
            filterRole: Qt.DisplayRole
            sortRole: Qt.DisplayRole
            filterCaseSensitivity: Qt.CaseInsensitive
            Component.onCompleted: sort(0)
        }

        boundsBehavior: Flickable.StopAtBounds

        delegate: Controls.ItemDelegate {
            icon: model && model.decoration
            text: model && model.display
            width: ListView.view.width

            onClicked: {
                const phoneNumbers = ContactUtils.phoneNumbers(model.personUri)
                if (phoneNumbers.length === 1) {
                    applicationWindow().call(phoneNumbers[0].normalizedNumber)
                } else {
                    const pop = callPopup.createObject(parent, {numbers: phoneNumbers, title: i18n("Select number to call")})
                    pop.onNumberSelected.connect(number => applicationWindow().call(number))
                    pop.open()
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No contacts have a phone number set")
            icon.name: "contact-new-symbolic"
            visible: contactsList.count === 0
        }
    }
}
