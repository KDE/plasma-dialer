// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: navigationTabBar

    function currentPage() {
        return applicationWindow().pageStack.currentItem
    }
    
    function getHistoryPage() {
        return applicationWindow().getPage("History");
    }
    function getContactsPage() {
        return applicationWindow().getPage("Contacts");
    }
    function getDialerPage() {
        return applicationWindow().getPage("Dialer");
    }

    // workaround to set default page since there's error:
    // NavigationTabBar.qml:235: TypeError: Cannot read property 'tabIndex' of null
    // during init actions sorting
    Connections {
        target: contentItem
        function onChildrenChanged() {
            if (contentItem.children.length === Object.keys(navigationTabBar.actions).length) {
                applicationWindow().switchToPage(getDialerPage(), 0);
            }
        }
    }

    actions: [
        Kirigami.Action {
            iconName: "clock"
            text: i18n("History")
            property bool opened: getHistoryPage() === currentPage()
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(getHistoryPage(), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "view-pim-contacts"
            text: i18n("Contacts")
            property bool opened: getContactsPage() === currentPage()
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(getContactsPage(), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "call-start"
            text: i18n("Dialer")
            property bool opened: getDialerPage() === currentPage()
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(getDialerPage(), 0);
                }
            }
        }
    ]
} 
