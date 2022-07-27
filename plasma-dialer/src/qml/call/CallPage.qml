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
    id: callPage

    function activeDeviceUni() {
        return applicationWindow().selectModem()
    }
    function activeCallUni() {
        return ActiveCallModel.activeCallUni()
    }

    property bool callActive: ActiveCallModel.active
    property int callState: ActiveCallModel.callState
    property int callDuration: ActiveCallModel.callDuration

    title: i18n("Active call list")

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
        if(h === 0) return '' + m + ':' + s;
        return '' + h + ':' + m + ':' + s;
    }

    Connections {
        target: DialerUtils
        function onMuteChanged(muted) {
            muteButton.toggledOn = muted
        }
        function onSpeakerModeChanged(enabled) {
            speakerButton.toggledOn = enabled
        }
    }

    ColumnLayout {
        id: activeCallUi
        spacing: Kirigami.Units.largeSpacing

        anchors {
            fill: parent
            margins: Kirigami.Units.largeSpacing
        }

        Controls.SwipeView {
            id: activeCallSwipeView

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: parent.height / 2

            ActiveCallView {
                id: activeCallView
            }

            Dialpad {
                id: dialPad
                focus: true
            }
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
        
        // time spent on call
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
            text: {
                if (callState === DialerTypes.CallState.Dialing || callState === DialerTypes.CallState.RingingOut) {
                    return i18n("Calling...");
                } else if (callDuration > 0) {
                    return secondsToTimeString(callDuration);
                } else {
                    return '';
                }
            }
            visible: text !== ""
        }

        // controls
        RowLayout {
            opacity: callActive ? 1 : 0
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3.5
            id: buttonRow
            
            spacing: Kirigami.Units.smallSpacing
            
            CallPageButton {
                id: dialerButton
                Layout.fillHeight: true
                Layout.fillWidth: true
                
                iconSource: "input-dialpad-symbolic"
                text: i18n("Keypad")
                
                onClicked: switchToogle()
                toggledOn: (activeCallSwipeView.currentIndex == 1)

                function switchToogle() {
                    // activeCallSwipeView: 0 is ActiveCallView, 1 is Dialpad
                    if (toggledOn) {
                        activeCallSwipeView.currentIndex = 0
                    } else {
                        activeCallSwipeView.currentIndex = 1
                    }
                }
            }
            CallPageButton {
                id: speakerButton
                Layout.fillHeight: true
                Layout.fillWidth: true
                
                iconSource: "audio-speakers-symbolic"
                text: i18n("Speaker")
                
                onClicked: {
                    const speakerMode = !toggledOn
                    DialerUtils.setSpeakerMode(speakerMode);
                }
            }
            CallPageButton {
                id: muteButton

                Layout.fillHeight: true
                Layout.fillWidth: true

                iconSource: toggledOn ? "microphone-sensitivity-muted-symbolic" : "microphone-sensitivity-high-symbolic"
                text: i18n("Mute")

                onClicked: {
                    const micMute = !toggledOn
                    DialerUtils.setMute(micMute)
                }
            }
        }

        Item {
            Layout.minimumHeight: Kirigami.Units.gridUnit * 5
            Layout.fillWidth: true

            AnswerSwipe {
                anchors.fill: parent
                visible: false // TODO
                onAccepted: {
                    CallUtils.accept(activeDeviceUni(), activeCallUni());
                }
                onRejected: {
                    CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                }
            }
            
            // end call button
            Controls.AbstractButton {
                id: endCallButton
                visible: callActive
                
                anchors.centerIn: parent
                width: Kirigami.Units.gridUnit * 3.5
                height: Kirigami.Units.gridUnit * 3.5
                
                onClicked: {
                    CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                }
                
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
