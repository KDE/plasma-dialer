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

Item {
    id: buttonRoot

    property var callback
    property string sub
    property alias source: icon.source
    property alias text: label.text
    property int size

    signal pressed()
    signal pressAndHold()

    Layout.fillWidth: true
    Layout.fillHeight: true

    Rectangle {
        anchors.fill: parent
        z: -1
        color: Kirigami.Theme.highlightColor
        radius: Kirigami.Units.smallSpacing
        opacity: mouse.pressed ? 0.4 : 0
    }

    Row {
        anchors.centerIn: parent

        Kirigami.Icon {
            id: icon

            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: buttonRoot.size || buttonRoot.height * 0.6
        }

        Controls.Label {
            id: label

            height: buttonRoot.height
            anchors.verticalCenter: parent.verticalCenter
            fontSizeMode: Text.VerticalFit
        }

    }

    Controls.AbstractButton {
        id: mouse

        anchors.fill: parent
        onClicked: buttonRoot.pressed()
        onPressAndHold: buttonRoot.pressAndHold()
    }

}
