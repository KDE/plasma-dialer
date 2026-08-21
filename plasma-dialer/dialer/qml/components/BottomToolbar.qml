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
        return applicationWindow().pageStack.currentItem;
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

    visible: height !== 0

    onImplicitHeightChanged: {
        // If implicit height changes, make sure we animate to it
        if (shouldShow) {
            showAnim.restart();
        }
    }

    function applyShouldShow() {
        if (shouldShow) {
            hideAnim.stop();
            showAnim.restart();
        } else {
            showAnim.stop();
            hideAnim.restart();
        }
    }

    property bool shouldShow: (applicationWindow().pageStack.layers.depth <= 1 && applicationWindow().pageStack.depth <= 1)
    Component.onCompleted: applyShouldShow()
    onShouldShowChanged: applyShouldShow()

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
            property bool opened: getHistoryPage() === currentPage()

            icon.name: "clock"
            text: i18n("History")
            checked: opened
            onTriggered: {
                if (!opened)
                    applicationWindow().switchToPage(getHistoryPage());

            }
        },
        Kirigami.Action {
            property bool opened: getContactsPage() === currentPage()

            icon.name: "view-pim-contacts"
            text: i18n("Contacts")
            checked: opened
            onTriggered: {
                if (!opened)
                    applicationWindow().switchToPage(getContactsPage());

            }
        },
        Kirigami.Action {
            property bool opened: getDialerPage() === currentPage()

            icon.name: "call-start"
            text: i18n("Dialer")
            checked: opened
            onTriggered: {
                if (!opened)
                    applicationWindow().switchToPage(getDialerPage());

            }
        }
    ]
}
