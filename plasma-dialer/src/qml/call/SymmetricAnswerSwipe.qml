/*
 *   SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.7 as QQC2

import org.kde.kirigami 2.8 as Kirigami

Item {
    id: root

    signal accepted
    signal rejected

    Item {
        id: controlRectangle

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.gridUnit
        anchors.rightMargin: Kirigami.Units.gridUnit

        property bool swipeAccepted: false

        onSwipeAcceptedChanged: {
            if (swipeAccepted) {
                if (dragHandler.distance > 0) {
                    root.accepted()
                } else {
                    root.rejected()
                }
            }
        }

        Rectangle {
            anchors.left: (dragHandler.distance < 0) ? parent.left : callControlIcon.left
            anchors.right: (dragHandler.distance <= 0) ? callControlIcon.right : parent.right
            radius: height
            anchors.verticalCenter: parent.verticalCenter
            height: (dragHandler.distance === 0) ? width : parent.height
            property color controlColor: Kirigami.Theme.highlightColor
            property color positiveColor: Kirigami.Theme.positiveBackgroundColor
            property color negativeColor: Kirigami.Theme.negativeBackgroundColor
            color: Qt.hsla(controlColor.hslHue -
                           Math.abs(controlColor.hslHue - positiveColor.hslHue) * dragHandler.progress,
                           controlColor.hslSaturation,
                           controlColor.hslLightness,
                           controlColor.a)
        }

        Rectangle {
            anchors.centerIn: callControlIcon
            width: height
            height: Kirigami.Units.gridUnit * 3
            radius: height
            property color controlColor: Kirigami.Theme.highlightColor
            property color positiveColor: Kirigami.Theme.positiveBackgroundColor
            property color negativeColor: Kirigami.Theme.negativeBackgroundColor
            color: Qt.hsla(controlColor.hslHue -
                           Math.abs(controlColor.hslHue - positiveColor.hslHue) * dragHandler.progress,
                           controlColor.hslSaturation,
                           controlColor.hslLightness,
                           controlColor.a)
        }

        Kirigami.Icon {
            id: callControlIcon

            width: height
            height: Kirigami.Units.gridUnit * 3

            property real leftSpacing: (controlRectangle.width - width) / 2

            property bool resting: x === leftSpacing

            anchors.verticalCenter: parent.verticalCenter
            x: leftSpacing

            source: "transform-move-horizontal"

            function resetXPosition() {
                if (x !== leftSpacing) {
                    x = leftSpacing
                }
            }

            onXChanged: {
                if (!dragHandler.active) {
                    resetXPosition()
                }
            }

            onLeftSpacingChanged: {
                if (!dragHandler.active) {
                    resetXPosition()
                }
            }

            SequentialAnimation {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    target: callControlIcon;
                    property: "rotation";
                    from: -8; to: 8;
                    easing.type: Easing.OutInElastic;
                    duration: Kirigami.Units.veryLongDuration * 3
                }
                PauseAnimation { duration: Kirigami.Units.shortDuration}
            }
        }

        DragHandler {
            id: dragHandler

            property var dragAreaItem: controlRectangle
            property var dragItem: callControlIcon

            property real swipeAcceptThreshold: 0.25
            property real distance: 0

            property real progress: dragHandler.distance % dragHandler.swipeAcceptThreshold / dragHandler.swipeAcceptThreshold

            target: null

            function syncDragItemX() {
                const positionX = dragHandler.centroid.position.x
                if (Math.abs(progress) > 1) {
                    return
                }
                dragItem.x = positionX - dragItem.width / 2
            }

            onDistanceChanged: {
                if (distance === 0) {
                    dragItem.resetXPosition()
                    dragAreaItem.swipeAccepted = false
                }
            }

            onTranslationChanged: {
                syncDragItemX()

                distance = translation["x"] / dragAreaItem.width

                if (Math.abs(distance) > swipeAcceptThreshold) {
                    dragAreaItem.swipeAccepted = true
                    dragItem.resetXPosition()
                }
            }

            onGrabChanged: {
                syncDragItemX()
            }

            onActiveChanged: {
                distance = 0
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.gridUnit
        anchors.rightMargin: Kirigami.Units.gridUnit

        Item {
            Layout.minimumWidth: height
            Layout.fillHeight: true
            visible: dragHandler.distance <= 0
            Kirigami.Icon {
                anchors.centerIn: parent
                property color callEndColor: Kirigami.Theme.negativeBackgroundColor
                source: (dragHandler.distance === 0) ? "call-stop" : "call-stop-symbolic"
                color: (dragHandler.distance === 0) ? callEndColor : Kirigami.Theme.textColor
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            Layout.minimumWidth: height
            Layout.fillHeight: true
            visible: dragHandler.distance >= 0
            Kirigami.Icon {
                anchors.centerIn: parent
                property color callStartColor: Kirigami.Theme.positiveBackgroundColor
                // Workaround since positiveBackgroundColor is too dark for the dark theme
                property color callStartColorWA: Qt.lighter(callStartColor, 3)
                source: (dragHandler.distance === 0) ? "call-start" : "call-start-symbolic"
                color: (dragHandler.distance === 0) ? callStartColorWA : Kirigami.Theme.textColor
            }
        }
    }
}
