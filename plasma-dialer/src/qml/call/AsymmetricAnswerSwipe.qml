/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
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

    signal accepted()
    signal rejected()

    Layout.minimumHeight: Kirigami.Units.gridUnit * 3
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: -parent.height
        anchors.rightMargin: -parent.height
        spacing: Kirigami.Units.largeSpacing * 4

        QQC2.AbstractButton {
            id: endCallButton

            Layout.minimumWidth: parent.width * 0.3
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                root.rejected();
            }

            background: Rectangle {
                radius: height
                color: Kirigami.Theme.negativeBackgroundColor
                opacity: endCallButton.pressed ? 0.5 : 1

                Kirigami.Icon {
                    id: endCallIcon

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: Kirigami.Units.largeSpacing * 2
                    source: "call-end-symbolic"

                    SequentialAnimation {
                        running: true
                        loops: Animation.Infinite

                        NumberAnimation {
                            target: endCallIcon
                            property: "rotation"
                            from: -8
                            to: 8
                            easing.type: Easing.OutInElastic
                            duration: Kirigami.Units.veryLongDuration * 3
                        }

                        PauseAnimation {
                            duration: Kirigami.Units.shortDuration
                        }

                    }

                }

            }

        }

        Rectangle {
            id: acceptRectangle

            property bool swipeAccepted: false

            Layout.minimumWidth: parent.width * 0.6
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: height
            color: Qt.lighter(Kirigami.Theme.positiveBackgroundColor, 1 + dragHandler.distance / dragHandler.swipeAcceptThreshold)
            onSwipeAcceptedChanged: {
                if (swipeAccepted)
                    root.accepted();

            }

            QQC2.Label {
                id: acceptLabel

                property real startX: callAcceptIcon.leftSpacing + callAcceptIcon.width + Kirigami.Units.smallSpacing
                property real parentWidth: parent.width
                property bool resting: callAcceptIcon.resting

                anchors.verticalCenter: parent.verticalCenter
                color: Kirigami.Theme.highlightedTextColor
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                text: i18n("Swipe to accept")
                visible: resting

                SequentialAnimation {
                    running: acceptLabel.resting
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: acceptLabel
                        property: "x"
                        from: acceptLabel.startX
                        to: acceptLabel.startX + Kirigami.Units.gridUnit * 2
                        easing.type: Easing.InBounce
                        duration: Kirigami.Units.veryLongDuration * 2
                    }

                    NumberAnimation {
                        target: acceptLabel
                        property: "x"
                        from: acceptLabel.startX + Kirigami.Units.gridUnit * 2
                        to: acceptLabel.startX
                        easing.type: Easing.OutBounce
                        duration: Kirigami.Units.veryLongDuration * 2
                    }

                    PauseAnimation {
                        duration: Kirigami.Units.shortDuration
                    }

                }

            }

            Kirigami.Icon {
                id: callAcceptIcon

                property real leftSpacing: Kirigami.Units.largeSpacing * 2
                property bool resting: x === leftSpacing

                function resetXPosition() {
                    x = leftSpacing;
                }

                anchors.verticalCenter: parent.verticalCenter
                x: leftSpacing
                source: "call-start-symbolic"
                onXChanged: {
                    if (!dragHandler.active)
                        resetXPosition();

                }

                SequentialAnimation {
                    running: true
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: callAcceptIcon
                        property: "rotation"
                        from: -8
                        to: 8
                        easing.type: Easing.OutInElastic
                        duration: Kirigami.Units.veryLongDuration * 3
                    }

                    PauseAnimation {
                        duration: Kirigami.Units.shortDuration
                    }

                }

            }

            DragHandler {
                id: dragHandler

                property var dragAreaItem: acceptRectangle
                property var dragItem: callAcceptIcon
                property real swipeAcceptThreshold: 0.33
                property real distance: 0

                function syncDragItemX() {
                    const positionX = dragHandler.centroid.position.x;
                    if (positionX < dragItem.leftSpacing)
                        return ;

                    dragItem.x = positionX - dragItem.width / 2;
                }

                target: null
                onDistanceChanged: {
                    if (distance === 0) {
                        dragItem.resetXPosition();
                        dragAreaItem.swipeAccepted = false;
                    }
                }
                onTranslationChanged: {
                    syncDragItemX();
                    distance = Math.abs(translation["x"]) / dragAreaItem.width;
                    if (distance > swipeAcceptThreshold) {
                        dragAreaItem.swipeAccepted = true;
                        dragItem.resetXPosition();
                    }
                }
                onGrabChanged: {
                    syncDragItemX();
                }
                onActiveChanged: {
                    distance = 0;
                }
            }

        }

    }

}
