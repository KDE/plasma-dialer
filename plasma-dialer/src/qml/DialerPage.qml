/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.9 as Kirigami

import org.kde.telephony 1.0

import "call"
import "dialpad"

Kirigami.Page {
    id: dialer

    property alias numberEntryText: statusLabel.text
    property alias pad: dialPad

    icon.name: "call-start"

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Connections {
        target: ActiveCallModel

        function onCallStateChanged() {
            if (ActiveCallModel.callState === DialerTypes.CallState.Active ||
                ActiveCallModel.callState === DialerTypes.CallState.Terminated) {
                if (pageStack.layers.currentItem === getPage("Incoming")) {
                    pageStack.layers.pop()
                    if (ActiveCallModel.callState === DialerTypes.CallState.Active) {
                        applicationWindow().pageStack.layers.push(getPage("Call"), 1)
                    }
                }
            }
        }

        function onActiveChanged() {
            if (ActiveCallModel.callState === DialerTypes.CallState.RingingIn) {
                const incomingPage = getPage("Incoming")
                if (ActiveCallModel.active) {
                    applicationWindow().pageStack.layers.push(incomingPage, 1)
                } else {
                    if (pageStack.layers.currentItem === incomingPage) {
                        pageStack.layers.pop()
                    }
                }
            } else {
                const callPage = getPage("Call")
                if (ActiveCallModel.active) {
                    applicationWindow().pageStack.layers.push(callPage, 1)
                } else {
                    if (pageStack.layers.currentItem === callPage) {
                        pageStack.layers.pop()
                    }
                }
            }
        }
    }
    
    ColumnLayout {
        id: dialPadArea
        anchors.fill: parent
        spacing: 0

        Rectangle {
            width: 720
            height: 405
            color: "#EAE9E9"

            ColumnLayout {
                RowLayout {
                    opacity: 0
                    Layout.topMargin: 152
                    Layout.leftMargin: 59

                    Kirigami.Icon {
                        source: ":icon-plus"
                        width: 20
                        height: 20
                    }
                    QQC2.Label {
                        text: "Создать контакт"
                    }
                }

                RowLayout {
                    Layout.topMargin: 87
//                    Layout.leftMargin: 107

                    QQC2.Label {
                        id: statusLabel
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        width: 620
                        height: 38
                        Layout.maximumWidth: width
                        Layout.minimumWidth: width
                        font.family: "Manrope"
                        font.pointSize: 26
                        font.weight: Font.Light
                        wrapMode: QQC2.Label.WrapAnywhere
                        maximumLineCount: 1
                        elide: Text.ElideRight
                        text: CallUtils.formatNumber(dialPad.number)
                    }

                    DialerIconButton {
                        id: delButton
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        width: 46
                        height: 31

                        enabled: statusLabel.text.length > 0
                        opacity: enabled ? 1 : 0.5
                        source: ":icon-clear"

                        onPressed: { dialPad.number = dialPad.number.slice(0, -1) }
                        onPressAndHold: {  dialPad.number = "" }

                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            Kirigami.Theme.backgroundColor: "#437431"
            color: Kirigami.Theme.backgroundColor
            width: 720
            height: 1195

            Dialpad {
                id: dialPad
                anchors.fill: parent
                anchors.bottomMargin: Kirigami.Units.largeSpacing * 2
                anchors.topMargin: Kirigami.Units.largeSpacing * 2
                anchors.leftMargin: Kirigami.Units.largeSpacing * 3
                anchors.rightMargin: Kirigami.Units.largeSpacing * 3
                focus: true
            }
        }
    }
}
