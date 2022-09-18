/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
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
                        NumberAnimation { target: endCallIcon; property: "rotation"
                            from: -8; to: 8;
                            easing.type: Easing.OutInElastic; duration: Kirigami.Units.veryLongDuration * 3
                        }
                        PauseAnimation { duration: Kirigami.Units.shortDuration}
                    }
                }
            }
            onClicked: {
                root.rejected()
            }
        }

        Rectangle {
            id: acceptRectangle
            Layout.minimumWidth: parent.width * 0.6
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: height

            property bool swipeAccepted: false

            color: Qt.lighter(Kirigami.Theme.positiveBackgroundColor, 1 + acceptMouseArea.swipePath)

            onSwipeAcceptedChanged: {
                if (swipeAccepted) {
                    root.accepted()
                }
            }

            // re-init after the previous show
            onVisibleChanged: {
                if (visible) {
                    swipeAccepted = false
                }
            }

            QQC2.Label {
                id: acceptLabel
                property real startX: callAcceptIcon.leftSpacing + callAcceptIcon.width + Kirigami.Units.smallSpacing
                property real parentWidth: parent.width

                anchors.verticalCenter: parent.verticalCenter
                color: Kirigami.Theme.highlightedTextColor
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                text: i18n("Swipe right to accept")
                property bool resting: acceptMouseArea.swipePath == 0
                visible: resting

                SequentialAnimation {
                    running: acceptLabel.resting
                    loops: Animation.Infinite
                    NumberAnimation { target: acceptLabel; property: "x"
                        from: acceptLabel.startX; to: acceptLabel.startX + Kirigami.Units.gridUnit * 2
                        easing.type: Easing.InBounce; duration: Kirigami.Units.veryLongDuration * 2
                    }
                    NumberAnimation { target: acceptLabel; property: "x";
                        from: acceptLabel.startX + Kirigami.Units.gridUnit * 2; to: acceptLabel.startX
                        easing.type: Easing.OutBounce; duration: Kirigami.Units.veryLongDuration * 2
                    }
                    PauseAnimation { duration: Kirigami.Units.shortDuration}
                }
            }

            Kirigami.Icon {
                id: callAcceptIcon

                anchors.verticalCenter: parent.verticalCenter
                property real leftSpacing: Kirigami.Units.largeSpacing * 2
                x: leftSpacing
                source: "call-start-symbolic"
                opacity: 1 - acceptMouseArea.swipePath

                SequentialAnimation {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation { target: callAcceptIcon; property: "rotation"; from: -8; to: 8; easing.type: Easing.OutInElastic; duration: Kirigami.Units.veryLongDuration * 3}
                    PauseAnimation { duration: Kirigami.Units.shortDuration}
                }

                // re-init after the previous show
                onVisibleChanged: {
                    if (visible) {
                        x = leftSpacing
                    }
                }

                Connections {
                    target: acceptMouseArea
                    function onMouseXChanged(mouse) {
                        if (mouse.x < callAcceptIcon.leftSpacing) {
                            return
                        }
                        if (mouse.x > acceptRectangle.width - callAcceptIcon.width) {
                            return
                        }
                        if (acceptRectangle.swipeAccepted) {
                            return
                        }

                        callAcceptIcon.x = mouse.x - (callAcceptIcon.width / 2)
                    }
                    function onReleased() {
                        if (acceptRectangle.swipeAccepted) {
                            return
                        }

                        callAcceptIcon.x = callAcceptIcon.leftSpacing
                    }
                    function onSwipeAccepted() {
                        callAcceptIcon.x = acceptRectangle.width + callAcceptIcon.width
                    }
                }
            }

            MouseArea {
                id: acceptMouseArea
                anchors.fill: parent
                property real swipePathStart: 0
                property real swipePath: 0
                property real swipeAcceptThreshold: 0.33
                signal swipeAccepted()
                function resetSwipePathVars() {
                    swipePath = 0
                    swipePathStart = 0
                }

                onMouseXChanged: {
                    swipePath = mouseX / width
                }
                onPressed: {
                    swipePathStart = mouseX / width
                }
                onReleased: {
                    resetSwipePathVars()
                    acceptRectangle.swipeAccepted = false
                }
                onSwipePathChanged: {
                    if ((swipePath - swipePathStart) >= swipeAcceptThreshold) {
                        swipeAccepted()
                    }
                }
                onSwipeAccepted: {
                    acceptRectangle.swipeAccepted = true
                    resetSwipePathVars()
                }
            }
        }
    }
}
