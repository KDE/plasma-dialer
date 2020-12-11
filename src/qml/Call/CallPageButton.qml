/*
 *   Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */ 

import QtQuick 2.0
import QtQuick.Controls 2.7 
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

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
