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
            iconName: "clock"
            text: i18n("History")
            property bool opened: applicationWindow().getPage("History") === applicationWindow().pageStack.currentItem
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(applicationWindow().getPage("History"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "view-pim-contacts"
            text: i18n("Contacts")
            property bool opened: applicationWindow().getPage("Contacts") === applicationWindow().pageStack.currentItem
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(applicationWindow().getPage("Contacts"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "call-start"
            text: i18n("Dialer")
            property bool opened: applicationWindow().getPage("Dialer") === applicationWindow().pageStack.currentItem
            checked: opened
            onTriggered: {
                if (!opened) {
                    applicationWindow().switchToPage(applicationWindow().getPage("Dialer"), 0);
                }
            }
        }
    ]
} 
