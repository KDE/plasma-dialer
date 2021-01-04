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

GridLayout {
    id: pad
    columns: 3
    rowSpacing: 10
    columnSpacing: 10

    property string number
    property bool showBottomRow: true

    DialerButton { id: one; text: "1"; onClicked: pad.number += text }
    DialerButton { text: "2"; sub: "ABC"; onClicked: pad.number += text }
    DialerButton { text: "3"; sub: "DEF"; onClicked: pad.number += text }

    DialerButton { text: "4"; sub: "GHI"; onClicked: pad.number += text }
    DialerButton { text: "5"; sub: "JKL"; onClicked: pad.number += text }
    DialerButton { text: "6"; sub: "MNO"; onClicked: pad.number += text }

    DialerButton { text: "7"; sub: "PQRS"; onClicked: pad.number += text }
    DialerButton { text: "8"; sub: "TUV"; onClicked: pad.number += text }
    DialerButton { text: "9"; sub: "WXYZ"; onClicked: pad.number += text }

    DialerButton { display: "＊"; text: "*"; special: true; onClicked: pad.number += text }
    DialerButton { text: "0"; subdisplay: "＋"; sub: "+"; onClicked: pad.number += text }
    DialerButton { display: "＃"; text: "#"; special: true; onClicked: pad.number += text }

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
        
        enabled: status.text.length > 0
        opacity: enabled ? 1 : 0.8
        onClicked: call(status.text)
        
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

        enabled: status.text.length > 0
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
