/*
 *   SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.1
import org.kde.kirigami 2.12 as Kirigami
import org.kde.people 1.0 as KPeople

import org.kde.phone.dialer 1.0

Kirigami.ScrollablePage {
    title: i18n("Contacts")
    icon.name: "view-pim-contacts"

    header: ColumnLayout {
        anchors.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.smallSpacing
        
        InCallInlineMessage {
            id: inCall
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            dialerUtils: DialerUtils
        }
        
        Kirigami.SearchField {
            id: searchField
            onTextChanged: contactsProxyModel.setFilterFixedString(text)
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
        }
    }

    ListView {
        id: contactsList

        section.property: "display"
        section.criteria: ViewSection.FirstCharacter
        section.delegate: Kirigami.ListSectionHeader {
            text: section
        }
        clip: true

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

        Component {
            id: contactListDelegate

            Kirigami.BasicListItem {
                icon: model && model.decoration
                label: model && model.display
                onClicked: DialerUtils.dial(model.phoneNumber)
            }
        }

        delegate: Kirigami.DelegateRecycler {
            width: parent ? parent.width : 0
            sourceComponent: contactListDelegate
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No contacts")
            icon.name: "contact-new-symbolic"
            visible: contactsList.count === 0
        }
    }
}
