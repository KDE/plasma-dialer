/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.8 as Kirigami

MouseArea {
    id: root

    signal accepted
    signal rejected

    Layout.minimumHeight: Kirigami.Units.gridUnit * 5
    Layout.fillWidth: true
    property int handlePosition: (answerHandle.x + answerHandle.width/2)
    drag {
        target: answerHandle
        axis: Drag.XAxis
        minimumX: 0
        maximumX: width - answerHandle.width
    }
    Rectangle {
        anchors.fill: parent
        radius: height
        color: Qt.rgba((handlePosition > root.width/2 ? 0.6 : 0)+0.2, (handlePosition < root.width/2 ? 0.6 : 0)+0.2, 0.2, Math.abs(handlePosition - (root.width/2)) / answerHandle.width/2);

        Kirigami.Icon {
            source: "call-start"
            width: icon.width
            height: width
            smooth: true
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: Kirigami.Units.largeSpacing
            }
        }
        Kirigami.Icon {
            source: "call-stop"
            width: icon.width
            height: width
            smooth: true
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                leftMargin: Kirigami.Units.largeSpacing
            }
        }

        Rectangle {
            id: answerHandle
            x: parent.width/2 - width/2
            height: parent.height
            width: height
            radius: width
            color: Qt.rgba(0.2, 0.8, 0.2, 1)
            Kirigami.Icon {
                id: icon
                source: "call-start"
                width: parent.width * 0.7
                height: width
                smooth: true
                anchors.centerIn: parent
            }
            Behavior on x {
                enabled: root.pressed
                XAnimator {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
    onReleased: {
        if (answerHandle.x <= answerHandle.width) {
            root.accepted();
        } else if (answerHandle.x + answerHandle.width >= root.width - answerHandle.width) {
            root.rejected();
        }

        answerHandle.x = width/2 - answerHandle.width/2
    }
}
