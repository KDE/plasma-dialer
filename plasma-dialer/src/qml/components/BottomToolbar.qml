// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

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

    actions: [
        Kirigami.Action {
            id: historyAction
            text: i18n("History")
            property bool opened: getHistoryPage() === currentPage()
            checked: opened
            enabled: !applicationWindow().lockscreenMode
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(getHistoryPage(), 0);
                }
            }
            Component.onCompleted: {
                // dynamic check could be dropped with KF6-only versions
                // https://invent.kde.org/frameworks/kirigami/-/merge_requests/942
                const name = "clock"
                if (historyAction.iconName !== undefined) {
                    historyAction.iconName = name
                } else {
                    historyAction.icon.name = name
                }
            }
        },
        Kirigami.Action {
            id: contactsAction
            text: i18n("Contacts")
            property bool opened: getContactsPage() === currentPage()
            checked: opened
            enabled: !applicationWindow().lockscreenMode
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(getContactsPage(), 0);
                }
            }
            Component.onCompleted: {
                // dynamic check could be dropped with KF6-only versions
                // https://invent.kde.org/frameworks/kirigami/-/merge_requests/942
                const name = "view-pim-contacts"
                if (contactsAction.iconName !== undefined) {
                    contactsAction.iconName = name
                } else {
                    contactsAction.icon.name = name
                }
            }
        },
        Kirigami.Action {
            id: dialerAction
            text: i18n("Dialer")
            property bool opened: getDialerPage() === currentPage()
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(getDialerPage(), 0);
                }
            }
            Component.onCompleted: {
                // dynamic check could be dropped with KF6-only versions
                // https://invent.kde.org/frameworks/kirigami/-/merge_requests/942
                const name = "call-start"
                if (dialerAction.iconName !== undefined) {
                    dialerAction.iconName = name
                } else {
                    dialerAction.icon.name = name
                }
            }
        }
    ]

    bottomPadding: applicationWindow().lockscreenMode ? (Kirigami.Units.gridUnit * 2) : 0

    Component.onCompleted: {
        if (applicationWindow().lockscreenMode) {
            let closeButtonBackground = Qt.createComponent("qrc:/components/CloseButtonBackground.qml")
            background = closeButtonBackground.createObject(root);
        }
    }
} 
