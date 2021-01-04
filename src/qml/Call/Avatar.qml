/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.13 as Kirigami

Item {
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
            source: "im-user"
            anchors.centerIn: parent
            height: Math.min(parent.width / 2, parent.height / 2)
            width: height
            color: Kirigami.Theme.textColor
        }
    }
    DropShadow {
        anchors.fill: avatar
        source: avatar
        horizontalOffset: 0
        verticalOffset: 1
        radius: 5
        samples: 6
        color: Qt.darker(Kirigami.Theme.backgroundColor, 1.1)
    }
}
