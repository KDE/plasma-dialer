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

import "../Dialpad"
import org.kde.phone.dialer 1.0

Kirigami.Page {
    objectName: "callPage"
    id: callPage
    
    title: i18n("Calling %1", DialerUtils.callContactDisplayString)

    property int callStatus: DialerUtils.callState

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
        if(h === 0) return '' + m + ':' + s;
        return '' + h + ':' + m + ':' + s;
    }

    onCallStatusChanged: {
        if (callStatus !== DialerUtils.Active) {
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
            interactive: callStatus === DialerUtils.Active;
            RowLayout {
                id: topContents
                
                Avatar {
                    source: DialerUtils.callContactPicture
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

        // phone number/alias
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.15
            text: DialerUtils.callContactDisplayString
            font.bold: true
            visible: text != ""
        }
        
        // time spent on call
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
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
            opacity: callStatus === DialerUtils.Active ? 1 : 0
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
                    //toggledOn = !toggledOn TODO uncomment once mute is working
                    
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
                visible: callStatus === DialerUtils.Incoming
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
                visible: callStatus !== DialerUtils.Incoming
                
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
                    opacity: endCallButton.pressed ? 0.5 : 1
                    
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
