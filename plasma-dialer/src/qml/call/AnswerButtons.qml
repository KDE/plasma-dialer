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

    Layout.minimumHeight: Kirigami.Units.gridUnit * 3
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.gridUnit
        anchors.rightMargin: Kirigami.Units.gridUnit

        QQC2.AbstractButton {
            id: endCallButton
            Layout.minimumWidth: height
            Layout.fillHeight: true
            background: Rectangle {
                radius: height
                color: Kirigami.Theme.negativeBackgroundColor
                opacity: endCallButton.pressed ? 0.5 : 1
                Kirigami.Icon {
                    id: endCallIcon
                    anchors.centerIn: parent
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
        
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        QQC2.AbstractButton {
            id: acceptCallButton
            Layout.minimumWidth: height
            Layout.fillHeight: true
            background: Rectangle {
                radius: height
                color: Kirigami.Theme.positiveBackgroundColor
                opacity: acceptCallButton.pressed ? 0.5 : 1
                Kirigami.Icon {
                    id: acceptCallIcon
                    anchors.centerIn: parent
                    source: "call-start-symbolic"

                    SequentialAnimation {
                        running: true
                        loops: Animation.Infinite
                        NumberAnimation { target: acceptCallIcon; property: "rotation"
                            from: -8; to: 8;
                            easing.type: Easing.OutInElastic; duration: Kirigami.Units.veryLongDuration * 3
                        }
                        PauseAnimation { duration: Kirigami.Units.shortDuration}
                    }
                }
            }
            onClicked: {
                root.accepted()
            }
        }
    }
}
