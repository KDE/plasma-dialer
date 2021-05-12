/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.5 as Kirigami

import org.kde.phone.dialer 1.0

GridLayout {
    id: pad
    columns: 3
    rowSpacing: 10
    columnSpacing: 10

    property string number
    property bool showBottomRow: true
    property bool voicemailFail: false

    function onPadNumberPressed(number) {
        pad.voicemailFail = false
        if (DialerUtils.callState !== DialerUtils.Active) {
            pad.number += number
        } else {
            DialerUtils.sendDtmf(number)
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
        if (isSpecialCode(number)) {
            DialerUtils.initiateUssd(number)
        } else if (number === "*#06#") {
            DialerUtils.getImeis()
        } else {
            DialerUtils.dial(number)
        }
    }

    function callVoicemail() {
        var number = DialerUtils.getVoicemailNumber()
        if (number === "") {
            pad.voicemailFail = true
        } else {
            pad.voicemailFail = false;
            // voicemail number is real phone number and should not be a MMI code
            DialerUtils.dial(number)
        }
    }

    DialerButton {
        id: one
        text: "1"
        onClicked: onPadNumberPressed(text)
        onHeld: DialerUtils.callState !== DialerUtils.Active && callVoicemail()
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
    DialerButton { text: "0"; subdisplay: "＋"; sub: "+"; onClicked: onPadNumberPressed(text) }
    DialerButton { display: "＃"; text: "#"; special: true; onClicked: onPadNumberPressed(text); onHeld: onPadNumberPressed(text) }

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
        onClicked: onCallButtonPressed(statusLabel.text)
        
        background: Rectangle {
            anchors.centerIn: parent
            height: Kirigami.Units.gridUnit * 3.5
            width: height
            radius: height / 2
            
            color: Kirigami.Theme.highlightColor
            opacity: callButton.pressed ? 0.7 : 1
            
            Kirigami.Icon {
                source: "call-start"
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing
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
        onPressAndHold: { // clear
            pad.number = ""
        }
    }
}
