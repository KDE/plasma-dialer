/**
 *   Copyright 2014 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2014 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.LocalStorage 2.0

import org.kde.kirigami 2.13 as Kirigami

import org.kde.phone.dialer 1.0
import "Call"

Kirigami.ApplicationWindow {
    wideScreen: false
    id: root

    title: i18n("Dialer")
    
    width: 800
    height: 1080

    visible: false

    readonly property bool smallMode: root.height < Kirigami.Units.gridUnit * 20

    //keep track if we were visible when ringing
    property bool wasVisible
    //was the last call an incoming one?
    property bool isIncoming

    pageStack.initialPage: historyPage
    
    function switchToPage(page, depth) {
        while (pageStack.depth > depth) pageStack.pop()
        pageStack.push(page)
    }
    
    footer: BottomToolbar {
        visible: root.pageStack.layers.depth < 2
    }
    
    HistoryPage {
        id: historyPage
        visible: true
    }

    ContactsPage {
        id: contactsPage
        visible: false
    }

    DialerPage {
        id: dialerPage
        visible: false

        function onDisplayImeis(imeis) {
            imeiSheet.imeis = imeis
            imeiSheet.open()
        }
    }

    USSDSheet {
        id: ussdSheet
        onResponseReady: {
            // TODO: debug
            // DialerUtils.respondToUssd(response)
        }
    }

    ImeiSheet {
        id: imeiSheet
    }

    Component {
        id: callPage
        CallPage {}
    }

    Connections {
        target: DialerUtils

        function onMissedCallsActionTriggered() {
            root.visible = true;
        }
        function onCallEnded(number, duration, incoming) {
            var callType;
            if (incoming && duration == 0) {
                callType = DialerUtils.IncomingRejected;
            } else if (incoming && duration > 0) {
                callType = DialerUtils.IncomingAccepted;
            } else {
                callType = DialerUtils.Outgoing;
            }
            historyModel.addCall(number, duration, callType)
        }
        function onCallStateChanged(state) {
            if (DialerUtils.callState === DialerUtils.Active || DialerUtils.callState === DialerUtils.Dialing) {
                if (root.pageStack.layers.depth == 1) {
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

        function onNotificationReceivedFromUssd(message) {
            ussdSheet.showNotification(message)
        }

        function onRequestReceivedFromUssd(message) {
            ussdSheet.showNotification(message, true)
        }

        function onInitiateUssd(number) {
            ussdSheet.open()
        }

        function onDisplayImeis(imeis) {
            imeiSheet.imeis = imeis
            imeiSheet.open()
        }
    }

    onVisibleChanged: {
        //TODO
        //reset missed calls if the status is not STATUS_INCOMING when got visible
    }

    function call(number) {
        dialerPage.pad.number = number
        switchToPage(dialerPage, 0)
    }

    CallHistoryModel {
        id: historyModel
    }
}
