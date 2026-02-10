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

import org.kde.plasma.callscreen
import org.kde.plasma.dialer.shared

Kirigami.Page {
    id: callPage

    property bool callIncoming: ActiveCallModel.incoming
    property bool callActive: ActiveCallModel.active
    property bool inCall: ActiveCallModel.inCall
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

    title: i18n("Call")

    actions: [
        // Allow user to close window if the window is open and no call is ongoing
        Kirigami.Action {
            visible: applicationWindow().lockscreenMode
            text: i18n("Close")
            icon.name: "window-close"
            onTriggered: Qt.quit()
        }
    ]

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

        // controls
        RowLayout {
            id: buttonRow

            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3.5
            Layout.preferredHeight: dialerButton.implicitHeight
            spacing: Kirigami.Units.smallSpacing

            CallPageButton {
                id: dialerButton
                visible: callPage.inCall

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
                visible: callPage.inCall

                Layout.fillHeight: true
                Layout.fillWidth: true
                iconSource: "audio-speakers-symbolic"
                text: i18n("Speaker")
                toggledOn: DialerUtils.speakerMode
                onClicked: DialerUtils.setSpeakerMode(!toggledOn);
            }

            CallPageButton {
                id: muteButton
                visible: callPage.inCall

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
                active: callIncoming && !inCall
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

                visible: callPage.callActive && (callPage.inCall || !callPage.callIncoming)
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
