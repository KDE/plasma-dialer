/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami

Item {

    property alias source : theIcon.source

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumHeight: parent.height / 2
    //Kirigami.Avatar {
        //height: Math.min(parent.width / 1.5, parent.height / 1.5)
        //width: height
        //anchors.centerIn: parent
        //source: "im-user"
    //}
    Rectangle {
        id: avatar
        height: Math.min(parent.width / 1.5, parent.height / 1.5)
        width: height
        anchors.centerIn: parent
        radius: Kirigami.Units.largeSpacing
        color: Qt.lighter(Kirigami.Theme.backgroundColor, 1.1)
        
        Kirigami.Icon {
            id: theIcon
            anchors.centerIn: parent
            height: Math.min(parent.width / 2, parent.height / 2)
            width: height
        }
    }
    MultiEffect {
        anchors.fill: avatar
        source: avatar
        shadowEnabled: true
        shadowHorizontalOffset: 0
        shadowVerticalOffset: 1
        blurMax: 5
        shadowColor: Qt.darker(Kirigami.Theme.backgroundColor, 1.1)
    }
}
