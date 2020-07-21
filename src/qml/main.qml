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

    width: 800
    height: 1080

    visible: false

    readonly property bool smallMode: root.height < Kirigami.Units.gridUnit * 20

    //keep track if we were visible when ringing
    property bool wasVisible
    //was the last call an incoming one?
    property bool isIncoming

    Kirigami.SwipeNavigator {
        id: navigator
        anchors.fill: parent

        HistoryPage {}

        ContactsPage {}

        DialerPage {}
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
            if (isIncomingCall && callDuration == 0) {
                callType = DialerUtils.IncomingRejected;
            } else if (isIncomingCall && callDuration > 0) {
                callType = DialerUtils.IncomingAccepted;
            } else {
                callType = DialerUtils.Outgoing;
            }
            historyModel.addCall(callContactNumber, callDuration, callType)
        }
        function onCallStateChanged(state) {
            if (DialerUtils.callState === DialerUtils.Active || DialerUtils.callState === DialerUtils.Dialing) {
                if (navigator.layers.depth == 1) {
                    navigator.layers.push(callPage)
                    root.show()
                    root.requestActivate()
                }
            } else {
                if (navigator.layers.depth > 1) {
                    navigator.layers.clear()
                }
            }
        }
    }

    onVisibleChanged: {
        //TODO
        //reset missed calls if the status is not STATUS_INCOMING when got visible
    }

    function call(number) {
        DialerUtils.dial(number)
    }

    CallHistoryModel {
        id: historyModel
    }
}
