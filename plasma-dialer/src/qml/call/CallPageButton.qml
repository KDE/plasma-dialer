/*
 *   SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */ 

import QtQuick
import QtQuick.Controls 
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Rectangle {
    id: buttonRoot
    
    property bool toggledOn
    property string text
    property string iconSource
    
    signal clicked
    
    radius: Kirigami.Units.smallSpacing
    color: {
        if (mouseArea.pressed) {
            return Qt.darker(Kirigami.Theme.backgroundColor, 1.1);
        } else if (mouseArea.containsMouse) {
            return Qt.darker(Kirigami.Theme.backgroundColor, 1.03)
        } else if (buttonRoot.toggledOn) {
            return Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.7);
        } else {
            return Kirigami.Theme.backgroundColor;
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        
        onClicked: buttonRoot.clicked()
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing
        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredHeight: Kirigami.Units.gridUnit * 1.5
            Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5
            color: buttonRoot.toggledOn ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            source: buttonRoot.iconSource
        }
        Label {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            color: buttonRoot.toggledOn ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle
            text: buttonRoot.text
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.05
        }
    }
}
