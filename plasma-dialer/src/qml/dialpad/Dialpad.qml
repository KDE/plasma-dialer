/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.5 as Kirigami

import org.kde.telephony 1.0

GridLayout {
    id: pad
    columns: 3
    rowSpacing: 10
    columnSpacing: 10

    property string number
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

    Keys.onPressed: {
        if (event.key === Qt.Key_Backspace) {
            pad.number = pad.number.slice(0, -1)
        } else if (
            (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) &&
            statusLabel.text.length > 0
        ) {
            onCallButtonPressed(pad.number)
        } else if (["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "#", "+"].includes(event.text)) {
            onPadNumberPressed(event.text)
        }
    }

    DialerRosaButton { iconSource: ":btn-1"; iconSourcePressed: ":btn-1gr"; onPressed: onPadNumberPressed("1") }
    DialerRosaButton { iconSource: ":btn-2"; iconSourcePressed: ":btn-2gr"; onPressed: onPadNumberPressed("2") }
    DialerRosaButton { iconSource: ":btn-3"; iconSourcePressed: ":btn-3gr"; onPressed: onPadNumberPressed("3") }

    DialerRosaButton { iconSource: ":btn-4"; iconSourcePressed: ":btn-4gr"; onPressed: onPadNumberPressed("4") }
    DialerRosaButton { iconSource: ":btn-5"; iconSourcePressed: ":btn-5gr"; onPressed: onPadNumberPressed("5") }
    DialerRosaButton { iconSource: ":btn-6"; iconSourcePressed: ":btn-6gr"; onPressed: onPadNumberPressed("6") }

    DialerRosaButton { iconSource: ":btn-7"; iconSourcePressed: ":btn-7gr"; onPressed: onPadNumberPressed("7") }
    DialerRosaButton { iconSource: ":btn-8"; iconSourcePressed: ":btn-8gr"; onPressed: onPadNumberPressed("8") }
    DialerRosaButton { iconSource: ":btn-9"; iconSourcePressed: ":btn-9gr"; onPressed: onPadNumberPressed("9") }

    DialerRosaButton { iconSource: ":btn-star"; iconSourcePressed: ":btn-stargr"; onPressed: onPadNumberPressed("*") }
    DialerRosaButton { iconSource: ":btn-0"; iconSourcePressed: ":btn-0gr"; onPressed: onPadNumberPressed("0"); onPressAndHold: onPadNumberPressed("+") }
    DialerRosaButton { iconSource: ":btn-sharp"; iconSourcePressed: ":btn-sharpgr"; onPressed: onPadNumberPressed("#") }

    DialerRosaButton { iconSource: ":btn-contact-recent"; iconSourcePressed: ":btn-contact-recentgr"; onPressed: applicationWindow().switchToPage(applicationWindow().getPage("History"), 0) }

    Item {
        id: buttonRoot

        Layout.columnSpan: 2
        Layout.rowSpan: 1

        Layout.fillWidth: true
        Layout.fillHeight: true

        Rectangle {
            anchors.fill: parent
            z: -1
            color: Kirigami.Theme.highlightColor
            opacity: 0
        }

        Kirigami.Icon {
            id: icon
            source: mouse.pressed ? ":btn-callgr" : ":btn-call"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 404
            height: 164
        }

        Controls.AbstractButton {
            id: mouse
            anchors.fill: parent
            enabled: statusLabel.text.length > 0
            onClicked: onCallButtonPressed(pad.number)
        }
    }
}
