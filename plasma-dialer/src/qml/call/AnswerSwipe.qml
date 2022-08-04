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
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: Kirigami.Units.largeSpacing * 2
                    source: "call-end-symbolic"
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

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Kirigami.Theme.positiveBackgroundColor }
                GradientStop {
                    position: acceptRectangle.swipeAccepted ?
                                  1 :
                                  acceptMouseArea.mouseX / acceptRectangle.width
                    color: Qt.lighter(Qt.lighter(Kirigami.Theme.positiveBackgroundColor))
                    ParallelAnimation on position
                    {
                        loops: Animation.Infinite
                        running: !acceptMouseArea.pressed && !acceptRectangle.swipeAccepted

                        NumberAnimation {
                            from: 0.7
                            to: 1
                            duration: Kirigami.Units.veryLongDuration * 2
                        }
                    }
                }
            }

            QQC2.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -parent.height / 4
                anchors.verticalCenter: parent.verticalCenter
                color: Kirigami.Theme.highlightedTextColor
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                text: i18n("Swipe right to accept")
                visible: !acceptMouseArea.pressed && !acceptRectangle.swipeAccepted
            }

            Kirigami.Icon {
                id: callAcceptIcon

                anchors.verticalCenter: parent.verticalCenter
                property real leftSpacing: Kirigami.Units.largeSpacing * 2
                x: leftSpacing
                source: "call-start-symbolic"

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
                        callAcceptIcon.x = callAcceptIcon.x = acceptRectangle.width
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
                onMouseXChanged: {
                    swipePath = mouseX / width
                }
                onPressed: {
                    swipePathStart = mouseX / width
                }
                onReleased: {
                    swipePath = 0
                    acceptRectangle.swipeAccepted = false
                }
                onSwipePathChanged: {
                    if ((swipePath - swipePathStart) >= swipeAcceptThreshold) {
                        swipeAccepted()
                    }
                }
                onSwipeAccepted: {
                    acceptRectangle.swipeAccepted = true
                    root.accepted()
                }
            }
        }
    }
}
