// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: navigationTabBar

    property bool shouldShow: pageStack.layers.depth <= 1 && pageStack.depth <= 1
    onShouldShowChanged: {
        if (shouldShow) {
            hideAnim.stop();
            showAnim.restart();
        } else {
            showAnim.stop();
            hideAnim.restart();
        }
    }

    visible: height !== 0

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

    // animate showing and hiding of navbar
    ParallelAnimation {
        id: showAnim
        NumberAnimation {
            target: navigationTabBar
            property: "height"
            to: navigationTabBar.implicitHeight
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: navigationTabBar
            property: "opacity"
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        id: hideAnim
        NumberAnimation {
            target: navigationTabBar
            property: "opacity"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: navigationTabBar
            property: "height"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
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
