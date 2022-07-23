/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *   SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.7 as Controls
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

import org.kde.telephony 1.0

import "../dialpad"

Kirigami.Page {
    id: incomingPage

    function activeDeviceUni() {
        return applicationWindow().selectModem()
    }
    function activeCallUni() {
        return ActiveCallModel.activeCallUni()
    }

    property bool callActive: ActiveCallModel.active


    ColumnLayout {
        id: incomingCallUI
        spacing: Kirigami.Units.largeSpacing

        anchors {
            fill: parent
            margins: Kirigami.Units.largeSpacing
        }

        // phone number/alias
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.15
            text: ContactUtils.displayString("")
            font.bold: true
            visible: text != ""
        }

        Item {
            Layout.minimumHeight: Kirigami.Units.gridUnit * 5
            Layout.fillWidth: true

            //accept call button
            Controls.AbstractButton {
                id: acceptCallButton
                onClicked: {
                    CallUtils.accept(activeDeviceUni(), activeCallUni());
                }
                
                background: Rectangle {
                    anchors.centerIn: parent
                    height: Kirigami.Units.gridUnit * 3.5
                    width: height
                    radius: height / 2
                    
                    color: "green"
                    opacity: acceptCallButton.pressed ? 0.5 : 1
                    
                    Kirigami.Icon {
                        source: "call-start"
                        anchors.fill: parent
                        anchors.margins: Kirigami.Units.largeSpacing
                        color: "white"
                        isMask: true
                    }
                }
            }
        
            //decline call button
            Controls.AbstractButton {
                id: declineCallButton
                visible: callActive
                
                anchors.centerIn: parent
                width: Kirigami.Units.gridUnit * 3.5
                height: Kirigami.Units.gridUnit * 3.5
                
                onClicked: {
                    CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                    Qt.quit()
                }
                
                background: Rectangle {
                    anchors.centerIn: parent
                    height: Kirigami.Units.gridUnit * 3.5
                    width: height
                    radius: height / 2
                    
                    color: "red"
                    opacity: declineCallButton.pressed ? 0.5 : 1
                    
                    Kirigami.Icon {
                        source: "call-stop"
                        anchors.fill: parent
                        anchors.margins: Kirigami.Units.largeSpacing
                        color: "white"
                        isMask: true
                    }
                }
            }
        }
    }
}
