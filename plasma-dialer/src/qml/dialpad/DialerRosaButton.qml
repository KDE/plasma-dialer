/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.2 as Kirigami


Item {
    id: buttonRoot

    Layout.fillWidth: true
    Layout.fillHeight: true

    signal pressed()
    signal pressAndHold()

    property string iconSource
    property string iconSourcePressed

    Rectangle {
        anchors.fill: parent
        z: -1
        color: Kirigami.Theme.highlightColor
        opacity: 0
    }

    Kirigami.Icon {
        id: icon
        source: mouse.pressed ? iconSourcePressed : iconSource
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: 187
        height: 164
    }

    Controls.AbstractButton {
        id: mouse
        anchors.fill: parent
        onClicked: buttonRoot.pressed()
        onPressAndHold: buttonRoot.pressAndHold()
    }
}
