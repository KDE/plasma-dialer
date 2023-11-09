/*
 *   SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.1
import org.kde.kirigami 2.14 as Kirigami
import org.kde.people 1.0 as KPeople

import org.kde.telephony 1.0

import "call"

Kirigami.ScrollablePage {
    id: contactsPage
    title: i18n("Contacts")
    icon.name: "view-pim-contacts"

    // page animation
    property real yTranslate: 0
    
    Kirigami.Action {
        id: settingsAction
        displayHint: Kirigami.DisplayHint.IconOnly
        visible: !applicationWindow().isWidescreen
        enabled: !applicationWindow().lockscreenMode
        text: i18n("Settings")
        onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
    }

    Component.onCompleted: {
        // dynamic check could be dropped with KF6-only versions
        // https://invent.kde.org/frameworks/kirigami/-/merge_requests/986
        if (contactsPage.mainAction !== undefined) {
            contactsPage.mainAction = settingsAction
        } else {
            contactsPage.actions = settingsAction
        }
        // https://invent.kde.org/frameworks/kirigami/-/merge_requests/942
        const name = "settings-configure"
        if (settingsAction.iconName !== undefined) {
            settingsAction.iconName = name
        } else {
            settingsAction.icon.name = name
        }
    }

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
