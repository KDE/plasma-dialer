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
    bottomPadding: applicationWindow().lockscreenMode ? (Kirigami.Units.gridUnit * 2) : 0
    Component.onCompleted: {
        if (applicationWindow().lockscreenMode) {
            let closeButtonBackground = Qt.createComponent("qrc:/components/CloseButtonBackground.qml");
            background = closeButtonBackground.createObject(root);
        }
    }
    actions: [
        Kirigami.Action {
            property bool opened: getHistoryPage() === currentPage()

            icon.name: "clock"
            text: i18n("History")
            checked: opened
            enabled: !applicationWindow().lockscreenMode
            onTriggered: {
                if (!opened)
                    applicationWindow().switchToPage(getHistoryPage(), 0);

            }
        },
        Kirigami.Action {
            property bool opened: getContactsPage() === currentPage()

            icon.name: "view-pim-contacts"
            text: i18n("Contacts")
            checked: opened
            enabled: !applicationWindow().lockscreenMode
            onTriggered: {
                if (!opened)
                    applicationWindow().switchToPage(getContactsPage(), 0);

            }
        },
        Kirigami.Action {
            property bool opened: getDialerPage() === currentPage()

            icon.name: "call-start"
            text: i18n("Dialer")
            checked: opened
            onTriggered: {
                if (!opened)
                    applicationWindow().switchToPage(getDialerPage(), 0);

            }
        }
    ]
}
