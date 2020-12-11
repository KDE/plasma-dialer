/*
 *   Copyright 2014 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *   Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Controls 2.7 as Controls
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

import "../Dialpad"
import org.kde.phone.dialer 1.0

Kirigami.Page {
    objectName: "callPage"
    id: callPage

    property int status: DialerUtils.callState

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(h < 10) h = '0' + h;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
        return '' + h + ':' + m + ':' + s;
    }

    onStatusChanged: {
        if (status !== DialerUtils.Active) {
            dialerButton.toggledOn = false;
        }
    }

    ColumnLayout {
        id: activeCallUi
        spacing: Kirigami.Units.largeSpacing

        anchors {
            fill: parent
            margins: Kirigami.Units.largeSpacing
        }

        Flickable {
            id: topFlickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: parent.height / 2

            contentWidth: topContents.width
            contentHeight: topContents.height
            interactive: status === DialerUtils.Active;
            RowLayout {
                id: topContents
                
                Avatar {
                    Layout.minimumWidth: topFlickable.width
                    Layout.minimumHeight: topFlickable.height
                }
                
                Dialpad {
                    Layout.minimumWidth: topFlickable.width
                    Layout.minimumHeight: topFlickable.height
                    showBottomRow: false
                }
            }

            onMovingChanged: {
                var checked = contentX > topFlickable.width/2;

                if (checked) {
                    topSlideAnim.to = topFlickable.width;
                } else {
                    topSlideAnim.to = 0;
                }
                dialerButton.checked = checked;
                topSlideAnim.running = true;
            }
            PropertyAnimation {
                id: topSlideAnim
                target: topFlickable
                properties: "contentX"
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Units.fontMetrics.pointSize * 2
            text: DialerUtils.callContactAlias
            visible: text != ""
        }
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            text: {
                if (DialerUtils.callState === DialerUtils.Dialing) {
                    return i18n("Calling...");
                } else if (DialerUtils.callDuration > 0) {
                    return secondsToTimeString(DialerUtils.callDuration);
                } else {
                    return '';
                }
            }
            visible: text !== ""
        }

        // controls
        RowLayout {
            opacity: status === DialerUtils.Active ? 1 : 0
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3.5
            id: buttonRow
            
            spacing: Kirigami.Units.smallSpacing
            
            CallPageButton {
                id: muteButton
                Layout.fillHeight: true
                Layout.fillWidth: true
                
                iconSource: toggledOn ? "microphone-sensitivity-muted-symbolic" : "microphone-sensitivity-high-symbolic"
                text: i18n("Mute")
                toggledOn: false
                
                onClicked: {
                    toggledOn = !toggledOn
                    // ofonoWrapper.isMicrophoneMuted = !ofonoWrapper.isMicrophoneMuted;
                }
//                 iconSource: ofonoWrapper.isMicrophoneMuted ? "audio-volume-muted" : "audio-volume-high" TODO
            }
            CallPageButton {
                id: dialerButton
                Layout.fillHeight: true
                Layout.fillWidth: true
                
                iconSource: "input-dialpad-symbolic"
                text: i18n("Keypad")
                toggledOn: false
                
                onClicked: toggledOn = !toggledOn
                
                onToggledOnChanged: {
                    if (toggledOn) {
                        topSlideAnim.to = topFlickable.width;
                    } else {
                        topSlideAnim.to = 0;
                    }
                    topSlideAnim.running = true;
                }
            }
            CallPageButton {
                id: speakerButton
                Layout.fillHeight: true
                Layout.fillWidth: true
                
                iconSource: "audio-speakers-symbolic"
                text: i18n("Speaker")
                toggledOn: false
                
                onClicked: toggledOn = !toggledOn
                
                // TODO functionality
            }
        }

        Item {
            Layout.minimumHeight: Kirigami.Units.gridUnit * 5
            Layout.fillWidth: true

            AnswerSwipe {
                anchors.fill: parent
                //STATUS_INCOMING
                visible: status === DialerUtils.Incoming
                onAccepted: {
                    DialerUtils.acceptCall();
                }
                onRejected: {
                    DialerUtils.rejectCall();
                }
            }
            
            // end call button
            Controls.AbstractButton {
                id: endCallButton
                //STATUS_ACTIVE
                visible: status !== DialerUtils.Incoming
                
                anchors.centerIn: parent
                width: Kirigami.Units.gridUnit * 3.5
                height: Kirigami.Units.gridUnit * 3.5
                
                onClicked: DialerUtils.hangUp()
                
                background: Rectangle {
                    anchors.centerIn: parent
                    height: Kirigami.Units.gridUnit * 3.5
                    width: height
                    radius: height / 2
                    
                    color: "red"
                    opacity: endCallButton.pressed ? 0.7 : 1
                    
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
