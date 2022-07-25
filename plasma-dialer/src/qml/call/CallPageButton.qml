/*
 *   SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */ 

import QtQuick 2.0
import QtQuick.Controls 2.7 
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.13 as Kirigami

Rectangle {
    id: buttonRoot
    
    property bool toggledOn: false
    property string text
    property string iconSource
    
    signal clicked
    
    radius: 25
    opacity: mouseArea.pressed ? 0.2 : 1

    color: "#437431"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        
        onClicked: {
            buttonRoot.clicked();
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: buttonRoot.iconSource
            Layout.minimumWidth: parent.width
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
