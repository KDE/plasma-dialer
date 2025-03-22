/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *   SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.telephony
import org.kde.plasma.dialer // Config
import "../dialpad"

Kirigami.Page {
    id: callPage

    property bool callIncoming: ActiveCallModel.incoming
    property bool callActive: ActiveCallModel.active
    property int callDuration: ActiveCallModel.duration
    property string callCommunicationWith: ActiveCallModel.communicationWith

    function activeDeviceUni() {
        return applicationWindow().selectModem();
    }

    function activeCallUni() {
        return ActiveCallModel.activeCallUni();
    }

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if (m < 10)
            m = '0' + m;

        if (s < 10)
            s = '0' + s;

        if (h === 0)
            return '' + m + ':' + s;

        return '' + h + ':' + m + ':' + s;
    }

    title: i18n("Active call list")

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

                showBottomRow: false
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
            text: ContactUtils.displayString(callCommunicationWith)
            font.bold: true
            visible: callActive
        }

        // time spent on call
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
            text: {
                if (callDuration > 0)
                    return secondsToTimeString(callDuration);

                if (callIncoming)
                    return i18n("Incoming...");

                if (callActive)
                    return i18n("Calling...");

                return '';
            }
            visible: text !== ""
        }

        // controls
        RowLayout {
            id: buttonRow

            opacity: callActive ? 1 : 0
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3.5
            spacing: Kirigami.Units.smallSpacing

            CallPageButton {
                id: dialerButton

                function switchToogle() {
                    // activeCallSwipeView: 0 is ActiveCallView, 1 is Dialpad
                    if (toggledOn)
                        activeCallSwipeView.currentIndex = 0;
                    else
                        activeCallSwipeView.currentIndex = 1;
                }

                Layout.fillHeight: true
                Layout.fillWidth: true
                iconSource: "input-dialpad-symbolic"
                text: i18n("Keypad")
                onClicked: switchToogle()
                toggledOn: (activeCallSwipeView.currentIndex == 1)
            }

            CallPageButton {
                id: speakerButton

                Layout.fillHeight: true
                Layout.fillWidth: true
                iconSource: "audio-speakers-symbolic"
                text: i18n("Speaker")
                toggledOn: DialerUtils.speakerMode
                onClicked: DialerUtils.setSpeakerMode(!toggledOn);
            }

            CallPageButton {
                id: muteButton

                Layout.fillHeight: true
                Layout.fillWidth: true
                iconSource: toggledOn ? "microphone-sensitivity-muted-symbolic" : "microphone-sensitivity-high-symbolic"
                text: i18n("Mute")
                toggledOn: DialerUtils.mute
                onClicked: DialerUtils.setMute(!toggledOn);
            }

        }

        Item {
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3.5
            Layout.fillWidth: true

            Loader {
                id: answerControlLoader

                anchors.fill: parent
                active: callIncoming && (callDuration == 0)
                sourceComponent: (Config.answerControl === 2) ? asymmetricAnswerSwipe : (Config.answerControl === 1) ? symmetricAnswerSwipe : answerButtons

                Connections {
                    function onAccepted() {
                        CallUtils.accept(activeDeviceUni(), activeCallUni());
                    }

                    function onRejected() {
                        CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                    }

                    target: answerControlLoader.item
                }

                Component {
                    id: answerButtons

                    AnswerButtons {
                    }

                }

                Component {
                    id: symmetricAnswerSwipe

                    SymmetricAnswerSwipe {
                    }

                }

                Component {
                    id: asymmetricAnswerSwipe

                    AsymmetricAnswerSwipe {
                    }

                }

            }

            // end call button
            Controls.AbstractButton {
                id: endCallButton

                visible: callActive && (callDuration > 0) || !callIncoming
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
