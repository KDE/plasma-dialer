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
import org.kde.people 1.0 as KPeople

import "../dialpad"

Kirigami.Page {
    id: callPage

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    KPeople.PersonsSortFilterProxyModel {
        id: contactsSearch
        sourceModel: KPeople.PersonsModel {
        }
        filterRole: KPeople.PersonsModel.PhoneNumberRole
        sortRole: Qt.DisplayRole
        filterCaseSensitivity: Qt.CaseInsensitive
    }

    function activeDeviceUni() {
        return applicationWindow().selectModem()
    }
    function activeCallUni() {
        return ActiveCallModel.activeCallUni()
    }

    property bool callActive: ActiveCallModel.active
    property int callState: ActiveCallModel.callState
    property int callDuration: ActiveCallModel.callDuration

    title: i18n("")

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
        if(h === 0) return '' + m + ':' + s;
        return '' + h + ':' + m + ':' + s;
    }

    function getColorForContact(ch) {
        var colors = {
            0: "green",
            1: "red",
            2: "yellow",
            3: "blue",
            4: "orange",
            5: "olive",
            6: "violet",
        };
        return colors[ch.charCodeAt(0) % 7];
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

        anchors {
            fill: parent
        }

        Rectangle {
            id: info
            Layout.fillWidth: true
            height: 690

            Rectangle {
                color: getColorForContact(contact.text.substr(0, 1).toUpperCase())
                y: 87
                width: 172
                height: 172

                anchors.horizontalCenter: info.horizontalCenter
                Controls.Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "Manrope"
                    font.pixelSize: 120
                    color: "#FFFFFF"
                    text: {
                        return contact.text.substr(0, 1).toUpperCase();
                    }
                }
            }
            // time spent on call
            Controls.Label {
                Layout.fillWidth: true
                Layout.minimumHeight: implicitHeight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                text: {
                    if (callState === DialerTypes.CallState.Dialing || callState === DialerTypes.CallState.RingingOut) {
                        return "Вызов...";
                    } else if (callDuration > 0) {
                        return secondsToTimeString(callDuration);
                    } else {
                        return '';
                    }
                }
                font.family: "Manrope"
                font.pixelSize: 26
                color: "#444444"
                y: 312
                anchors.horizontalCenter: info.horizontalCenter
            }
            // phone number/alias
            Controls.Label {
                id: phone
                Layout.fillWidth: true
                Layout.minimumHeight: implicitHeight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.family: "Manrope"
                font.pixelSize: 56
                lineHeight: 48
                minimumPixelSize: 56
                maximumLineCount: 3
                text: getPage("Dialer").pad.number
                color: "#444444"
                width: 620
                height: 120
                y: 402
                anchors.horizontalCenter: info.horizontalCenter
                Component.onCompleted: {
                    contactsSearch.setFilterFixedString(getPage("Dialer").pad.number);
                    contact.text = contactsSearch.data(contactsSearch.index(0, 0));
                }
            }
            // contact name
            Controls.Label {
                id: contact
                Layout.minimumHeight: implicitHeight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                maximumLineCount: 3

                width: 620
                height: 120

                font.family: "Manrope"
                font.pixelSize: 40
                color: "#444444"

                text: ContactUtils.displayString(phone.text)
                y: 503
                anchors.horizontalCenter: info.horizontalCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideMiddle
            }
        }

        // controls
        Rectangle {
            color: "#437431"

            Layout.fillWidth: true
            Layout.fillHeight: true

            CallPageButton {
                id: muteButton

                width: 296
                height: 164
                x: 50
                y: 75

                //iconSource: ":icons-mic"
                iconSource: ":btn-mic-on"
                text: i18n("Mute")

                onClicked: {
                    const micMute = !toggledOn;
                    DialerUtils.setMute(micMute);
                    iconSource = toggledOn ? ":btn-mic-off" : ":btn-mic-on";
                    muteLabel.text = toggledOn ? "Включить\nмикрофон" : "Выключить\nмикрофон";
                }
            }
            Controls.Label {
                id: muteLabel

                x: 50
                y: 253
                height: 80
                width: 296
                color: "#FFFFFF"

                font.family: "Manrope"
                font.pixelSize: 26
                text: {
                    "Выключить\nмикрофон"
                }

                anchors.horizontalCenter: info.horizontalCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideMiddle
                maximumLineCount: 2
                Layout.minimumHeight: implicitHeight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }

            CallPageButton {
                id: speakerButton
                width: 296
                height: 164
                x: 374
                y: 75
                
                //iconSource: ":icons-vol-mute"
                iconSource: ":btn-vol"
                text: i18n("Speaker")
                
                onClicked: {
                    const speakerMode = !toggledOn;
                    DialerUtils.setSpeakerMode(speakerMode);
                    iconSource = toggledOn ? ":btn-vol-on" : ":btn-vol";
                    speakerLabel.text = toggledOn ? "Выключить\nгромкую связь" : "Включить\nгромкую связь";
                }
            }

            Controls.Label {
                id: speakerLabel
                x: 374
                y: 253
                height: 80
                width: 296
                color: "#FFFFFF"

                font.family: "Manrope"
                font.pixelSize: 26
                text: {
                    "Включить\nгромкую связь"
                }
                anchors.horizontalCenter: info.horizontalCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideMiddle
                maximumLineCount: 2
                Layout.minimumHeight: implicitHeight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }

            CallPageButton {
                id: dialerButton

                width: 187
                height: 164
                x: 50
                y: 481

                //iconSource: ":icon-dialer"
                iconSource: ":btn-dialercontact-all"
                text: i18n("Keypad")

                onClicked: switchToogle()
                toggledOn: false

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
                id: endCallButton

                width: 404
                height: 164
                x: 266
                y: 481

                //iconSource: ":icon-handset-hangup"
                iconSource: ":btn-call-hangup"
                visible: callActive
                
                onClicked: {
                    CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                }
            }
        }
    }
}
