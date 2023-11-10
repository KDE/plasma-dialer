/*
 *   SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

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
            anchors.left: parent.left
            anchors.right: parent.right
            radius: height
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            color: Qt.darker(Kirigami.Theme.alternateBackgroundColor, 1.5)
        }

        Rectangle {
            visible: dragHandler.active
            property var dragItem: callControlItem
            anchors.left: (dragHandler.distance < 0) ? parent.left : dragItem.left
            anchors.right: (dragHandler.distance <= 0) ? dragItem.right : parent.right
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

        Item {
            id: callControlItem
            width: height
            height: Kirigami.Units.gridUnit * 3
            property real shakeX: 0

            property real leftSpacing: (controlRectangle.width - width) / 2

            property bool resting: x === leftSpacing

            SequentialAnimation {
                running: !dragHandler.active
                loops: Animation.Infinite
                NumberAnimation {
                    target: callControlItem;
                    property: "shakeX";
                    from: -4; to: 4;
                    easing.type: Easing.OutInElastic;
                    duration: Kirigami.Units.veryLongDuration * 3
                }
                PauseAnimation { duration: Kirigami.Units.shortDuration}
            }

            function resetXPosition() {
                if (x !== leftSpacing) {
                    x = leftSpacing
                    if (!dragAreaZone.pressed) {
                        dragHandler.enabled = false
                    }
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
        }

        Rectangle {
            x: callControlItem.x + callControlItem.shakeX
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: callControlItem.height
            radius: height
            property color controlColor: Kirigami.Theme.highlightColor
            property color positiveColor: Kirigami.Theme.positiveBackgroundColor
            property color negativeColor: Kirigami.Theme.negativeBackgroundColor
            color: Qt.hsla(controlColor.hslHue -
                           Math.abs(controlColor.hslHue - positiveColor.hslHue) * dragHandler.progress,
                           controlColor.hslSaturation,
                           controlColor.hslLightness,
                           controlColor.a)

            Kirigami.Icon {
                id: callControlIcon
                anchors.fill: parent
                anchors.centerIn: parent
                source: "transform-move-horizontal"
            }
        }


    }

    Item {
        id: dragAreaZone
        anchors.fill: parent
        property real thresholdScale: 1
        property bool pressed: mouseArea.pressed
        anchors.leftMargin: Kirigami.Units.gridUnit + height * thresholdScale
        anchors.rightMargin: Kirigami.Units.gridUnit + height * thresholdScale

        DragHandler {
            id: dragHandler

            property var dragAreaItem: controlRectangle
            property var dragItem: callControlItem

            property real swipeAcceptThreshold: 0.25
            property real distance: 0

            property real progress: dragHandler.distance % dragHandler.swipeAcceptThreshold / dragHandler.swipeAcceptThreshold

            target: null
            enabled: false
            yAxis.enabled: false

            function syncDragItemX() {
                const positionX = dragHandler.centroid.position.x
                if (Math.abs(progress) > 1) {
                    return
                }
                dragItem.x = positionX + dragItem.width / 2
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

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onPressed: {
                dragHandler.enabled = true
            }
            onReleased: {
                dragHandler.enabled = false
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
