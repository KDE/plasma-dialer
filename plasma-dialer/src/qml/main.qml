// SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
// SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.LocalStorage 2.0

import org.kde.kirigami 2.19 as Kirigami

import org.kde.telephony 1.0
import "Call"

Kirigami.ApplicationWindow {
    wideScreen: false
    id: root
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar

    title: i18n("Phone")
    
    width: 800
    height: 1080

    visible: true

    readonly property bool smallMode: root.height < Kirigami.Units.gridUnit * 20

    pageStack.initialPage: dialerPage
    
    function switchToPage(page, depth) {
        while (pageStack.depth > depth) pageStack.pop()
        pageStack.push(page)
        page.forceActiveFocus()
    }
    
    footer: Kirigami.NavigationTabBar {
        visible: root.pageStack.layers.depth < 2
        actions: [
            Kirigami.Action {
                iconName: "clock"
                text: i18n("History")
                property bool opened: historyPage.visible
                checked: opened
                onTriggered: {
                    if (!opened) {
                        root.switchToPage(historyPage, 0);
                    }
                }
            },
            Kirigami.Action {
                enabled: false // TODO
                iconName: "view-pim-contacts"
                text: i18n("Contacts")
                property bool opened: contactsPage.visible
                checked: opened
                onTriggered: {
                    if (!opened) {
                        root.switchToPage(contactsPage, 0);
                    }
                }
            },
            Kirigami.Action {
                iconName: "call-start"
                text: i18n("Dialer")
                property bool opened: dialerPage.visible
                checked: opened
                onTriggered: {
                    if (!opened) {
                        root.switchToPage(dialerPage, 0);
                    }
                }
            }
        ]
    }
    
    HistoryPage {
        id: historyPage
        visible: false
    }

    ContactsPage {
        id: contactsPage
        visible: false
    }

    DialerPage {
        id: dialerPage
        visible: true
    }

    USSDSheet {
        id: ussdSheet
        onResponseReady: {
            // TODO: debug
            // USSDUtils.respond(response)
        }
    }

    ImeiSheet {
        id: imeiSheet
        function show() {
            imeiSheet.imeis = DeviceUtils.equipmentIdentifiers()
            imeiSheet.open()
        }
    }

    Component {
        id: callPage
        CallPage {
        }
    }

    Connections {
        target: CallUtils

        function onMissedCallsActionTriggered() {
            root.visible = true;
        }

        function onCallStateChanged(state) {
            if (CallUtils.callState === DialerTypes.CallState.Active) {
                dialerPage.pad.number = ""
            }
            if (DialerUtils.callState === DialerTypes.CallState.Active
                    || DialerUtils.callState === DialerTypes.CallState.Dialing) {
                if (root.pageStack.layers.depth === 1) {
                    root.pageStack.layers.push(callPage)
                    root.show()
                    root.requestActivate()
                }
            } else {
                if (root.pageStack.layers.depth > 1) {
                    root.pageStack.layers.clear()
                }
            }
        }
    }
    
    Connections {
        target: UssdUtils

        function onNotificationReceived(deviceUni, message) {
            ussdSheet.showNotification(message)
        }

        function onRequestReceived(deviceUni, message) {
            ussdSheet.showNotification(message, true)
        }

        function onInitiated(deviceUni, command) {
            ussdSheet.open()
        }
    }

    onVisibleChanged: {
        //TODO
        //reset missed calls if the status is not STATUS_INCOMING when got visible
    }

    function selectModem() {
        const deviceUniList = DeviceUtils.deviceUniList()
        if (deviceUniList.length === 0) {
            console.warn("Modem devices not found")
            return ""
        }

        if (deviceUniList.length === 1) {
            return deviceUniList[0]
        }
        console.log("TODO: select device uni")
    }

    function call(number) {
        dialerPage.pad.number = number
        switchToPage(dialerPage, 0)
    }
}
