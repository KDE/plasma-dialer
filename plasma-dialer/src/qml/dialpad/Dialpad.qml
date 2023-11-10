/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.telephony

GridLayout {
    id: pad
    columns: 3
    rowSpacing: 10
    columnSpacing: 10

    property string number
    property bool showBottomRow: true
    property bool voicemailFail: false
    property bool callActive: ActiveCallModel.active

    function sendDtmf(tones) {
        const deviceUni = applicationWindow().selectModem()
        const callUni = ActiveCallModel.activeCallUni()
        CallUtils.sendDtmf(deviceUni, callUni, tones)
    }

    function onPadNumberPressed(number) {
        pad.voicemailFail = false
        if (callActive) {
            sendDtmf(number)
        } else {
            pad.number += number
        }
    }

    function isSpecialCode(number) {
        if (!number.startsWith("*")) {
            return false
        }
        if (!number.endsWith("#")) {
            return false
        }

        if (number === "*#06#") {
            return false
        }

        if (number === "*#07#") {
            console.log("TODO: show device info directly")
            return false
        }

        var simCodePrefixes = [ "**04*", "**042*", "**05*", "**052*" ]
        if (simCodePrefixes.some(simCodePrefix => number.startsWith(simCodePrefix))) {
            console.log("TODO: handle sim codes directly")
            return false
        }

        return true
    }

    function onCallButtonPressed(number) {
        const deviceUni = applicationWindow().selectModem()
        if (isSpecialCode(number)) {
            applicationWindow().ussdUserInitiated()
            UssdUtils.initiate(deviceUni, number)
        } else if (number === "*#06#") {
            imeiSheet.show()
        } else {
            CallUtils.dial(deviceUni, number)
        }
    }

    function callVoicemail() {
        var number = VoiceMailUtils.getVoicemailNumber()
        if (number === "") {
            pad.voicemailFail = true
        } else {
            pad.voicemailFail = false;
            // voicemail number is real phone number and should not be a MMI code
            var device = applicationWindow().selectModem()
            CallUtils.dial(device, number)
        }
    }

    function clear() {
        number = ""
    }

    DialerButton {
        id: one
        text: "1"
        onClicked: onPadNumberPressed(text)
        voicemail: showBottomRow // TODO: only show voicemail icon if voicemail number exists
        onHeld: !callActive && callVoicemail()
    }
    DialerButton { text: "2"; sub: "ABC"; onClicked: onPadNumberPressed(text) }
    DialerButton { text: "3"; sub: "DEF"; onClicked: onPadNumberPressed(text) }

    DialerButton { text: "4"; sub: "GHI"; onClicked: onPadNumberPressed(text) }
    DialerButton { text: "5"; sub: "JKL"; onClicked: onPadNumberPressed(text) }
    DialerButton { text: "6"; sub: "MNO"; onClicked: onPadNumberPressed(text) }

    DialerButton { text: "7"; sub: "PQRS"; onClicked: onPadNumberPressed(text) }
    DialerButton { text: "8"; sub: "TUV"; onClicked: onPadNumberPressed(text) }
    DialerButton { text: "9"; sub: "WXYZ"; onClicked: onPadNumberPressed(text) }

    DialerButton { display: "＊"; text: "*"; special: true; onClicked: onPadNumberPressed(text); onHeld: onPadNumberPressed(text) }
    DialerButton { text: "0"; subdisplay: "＋"; sub: "+"; onClicked: onPadNumberPressed(text); onHeld: onPadNumberPressed("+") }
    DialerButton { display: "＃"; text: "#"; special: true; onClicked: onPadNumberPressed(text); onHeld: onPadNumberPressed(text) }

    Keys.onPressed: (event)=> {
        if (event.key === Qt.Key_Backspace) {
            pad.number = pad.number.slice(0, -1)
        } else if (
            (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) &&
            pad.showBottomRow &&
            statusLabel.text.length > 0
        ) {
            onCallButtonPressed(pad.number)
        } else if (["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "#", "+"].includes(event.text)) {
            onPadNumberPressed(event.text)
        }
    }

    Item {
        visible: pad.showBottomRow
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    // call button
    Controls.AbstractButton {
        visible: pad.showBottomRow
        
        id: callButton
        Layout.fillWidth: true
        Layout.fillHeight: true
        
        enabled: pad.showBottomRow && statusLabel.text.length > 0
        opacity: enabled ? 1 : 0.8
        onClicked: onCallButtonPressed(pad.number)
        
        background: Rectangle {
            anchors.centerIn: parent
            height: parent.height
            width: height
            radius: height / 2
            
            color: Kirigami.Theme.highlightColor
            opacity: callButton.pressed ? 0.7 : 1
            
            Kirigami.Icon {
                source: "call-start"
                anchors.fill: parent
                anchors.margins: applicationWindow().smallMode ? Kirigami.Units.smallSpacing : Kirigami.Units.largeSpacing
                color: "white"
                isMask: true
            }
        }
    }
    
    DialerIconButton {
        visible: pad.showBottomRow
        
        id: delButton
        Layout.fillWidth: true
        Layout.fillHeight: true

        enabled: pad.showBottomRow && statusLabel.text.length > 0
        opacity: enabled ? 1 : 0.5
        source: "edit-clear"
        size: Kirigami.Units.gridUnit * 2
        onPressed: {
            pad.number = pad.number.slice(0, -1)
        }
        onPressAndHold: {
            clear()
        }
    }
}
