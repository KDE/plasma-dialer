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

    Rectangle {
        id: controlRectangle

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.gridUnit
        anchors.rightMargin: Kirigami.Units.gridUnit
        radius: height

        property color controlColor: Kirigami.Theme.highlightColor
        property color positiveColor: Kirigami.Theme.positiveBackgroundColor
        property color negativeColor: Kirigami.Theme.negativeBackgroundColor

        color: Qt.hsla(controlColor.hslHue -
                       Math.abs(controlColor.hslHue - positiveColor.hslHue) * dragHandler.progress,
                       controlColor.hslSaturation,
                       controlColor.hslLightness,
                       controlColor.a)

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

        Kirigami.Icon {
            id: callControlIcon

            property real leftSpacing: (controlRectangle.width - width) / 2

            property bool resting: x === leftSpacing

            anchors.verticalCenter: parent.verticalCenter
            x: leftSpacing

            source: "drag-handle-symbolic"

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

        Rectangle {
            Layout.minimumWidth: height
            Layout.fillHeight: true
            radius: height
            visible: dragHandler.distance <= 0
            property color callEndColor: Kirigami.Theme.negativeBackgroundColor
            color: Qt.hsla(callEndColor.hslHue,
                           callEndColor.hslSaturation,
                           callEndColor.hslLightness,
                           dragHandler.distance < 0 ?
                               callEndColor.a + dragHandler.progress :
                               callEndColor.a
                           )
            Kirigami.Icon {
                anchors.centerIn: parent
                source: "call-end-symbolic"
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            Layout.minimumWidth: height
            Layout.fillHeight: true
            radius: height
            visible: dragHandler.distance >= 0
            property color callStartColor: Kirigami.Theme.positiveBackgroundColor
            color: Qt.hsla(callStartColor.hslHue,
                           callStartColor.hslSaturation,
                           callStartColor.hslLightness,
                           dragHandler.distance > 0 ?
                               callStartColor.a - dragHandler.progress :
                               callStartColor.a
                           )
            Kirigami.Icon {
                anchors.centerIn: parent
                source: "call-start-symbolic"
            }
        }
    }
}
