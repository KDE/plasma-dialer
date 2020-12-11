/*
 *   Copyright 2014 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2014 Marco Martin <mart@kde.org>
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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls
import QtMultimedia 5.12

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
    property string sound
    property int size

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

    SoundEffect {
        id: playKeytone
        source: buttonRoot.sound
        volume: 0.5
        loops: SoundEffect.Infinite
    }
    
    Controls.AbstractButton {
        id: mouse
        anchors.fill: parent
        
        Timer { // allow time for sound to play if press/release is fast
            id: releaseTimer
            repeat: false
            onTriggered: playKeytone.stop()
        }
        
        property double pressTime: 0
        
        onPressed: {
            releaseTimer.stop();
            playKeytone.play();
            pressTime = new Date().getTime();
        }
        onCanceled: {
            let curTime = new Date().getTime();
            if ((curTime - pressTime) < 200) {
                releaseTimer.interval = 200 - (curTime - pressTime);
                releaseTimer.restart();
            } else {
                playKeytone.stop();
            }
        }
        
        onClicked: buttonRoot.pressed()
        onPressAndHold: buttonRoot.pressAndHold()
    }
}
