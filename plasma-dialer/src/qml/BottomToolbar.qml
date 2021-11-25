// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: navigationTabBar
    visible: appWindow.pageStack.layers.depth <= 1
    
    actions: [
        Kirigami.Action {
            iconName: "clock"
            text: i18n("History")
            property bool opened: appWindow.getPage("History") === appWindow.pageStack.currentItem
            checked: opened
            onTriggered: {
                if (!opened) {
                    appWindow.switchToPage(appWindow.getPage("History"), 0);
                }
            }
        },
        Kirigami.Action {
            enabled: false // TODO
            iconName: "view-pim-contacts"
            text: i18n("Contacts")
            property bool opened: appWindow.getPage("Contacts") === appWindow.pageStack.currentItem
            checked: opened
            onTriggered: {
                if (!opened) {
                    appWindow.switchToPage(appWindow.getPage("Contacts"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "call-start"
            text: i18n("Dialer")
            property bool opened: appWindow.getPage("Dialer") === appWindow.pageStack.currentItem
            checked: opened
            onTriggered: {
                if (!opened) {
                    appWindow.switchToPage(appWindow.getPage("Dialer"), 0);
                }
            }
        }
    ]
} 
