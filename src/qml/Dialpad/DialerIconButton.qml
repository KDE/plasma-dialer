/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls
import QtFeedback 5.0

import org.kde.kirigami 2.2 as Kirigami


Item {
    id: buttonRoot

    Layout.fillWidth: true
    Layout.fillHeight: true

    signal pressed()
    signal pressAndHold()

    property var callback
    property string sub
    property alias source: icon.source
    property alias text: label.text
    property int size

    Rectangle {
        anchors.fill: parent
        z: -1
        color: Kirigami.Theme.highlightColor
        radius: Kirigami.Units.smallSpacing
        opacity: mouse.pressed ? 0.4 : 0
    }

    // vibration
    HapticsEffect {
        id: vibrate
        attackIntensity: 0.0
        attackTime: 0
        fadeTime: 0
        fadeIntensity: 0.0
        intensity: 0.5
        duration: Kirigami.Units.shortDuration
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
        onPressed: vibrate.start()
        onClicked: buttonRoot.pressed()
        onPressAndHold: {
            buttonRoot.pressAndHold();
            vibrate.start();
        }
    }
}
