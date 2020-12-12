/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
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
