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
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0


    function activeDeviceUni() {
        return applicationWindow().selectModem()
    }
    function activeCallUni() {
        return ActiveCallModel.activeCallUni()
    }

    property bool callActive: ActiveCallModel.active


    Rectangle{
        id:mainRect
        anchors.fill: parent

    Rectangle {
        id: topSide
        color: "#C4C4C4"
        parent: mainRect
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        height: parent.height / 2
        width: parent.width
        // phone number/alias
        Controls.Label {
            Layout.fillWidth: true
            anchors.centerIn: parent
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.15
            text: ContactUtils.displayString("somebody calls")
            font.bold: true
            visible: text != ""
        }
    }

    Rectangle{
        id: bottomSide
        parent: mainRect
        color: "#437431"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        height: parent.height / 2
        width: parent.width

        //accept call button
        Controls.AbstractButton {
            id: acceptCallButton
            parent: bottomSide
            anchors.centerIn: parent
            height: 164
            width: 404

            onClicked: {
                CallUtils.accept(activeDeviceUni(), activeCallUni());
            }

            background: Rectangle {
                anchors.centerIn: parent
                height: 164
                width: 404
                radius: 25

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
            anchors.top: acceptCallButton.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 5
            height: 164
            width: 404
            onClicked: {
                CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                Qt.quit()
            }

            background: Rectangle {
                anchors.centerIn: parent
                height: 164
                width: 404
                radius: 25
                

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
        } //button

    } //rectangle
    }
} //page
