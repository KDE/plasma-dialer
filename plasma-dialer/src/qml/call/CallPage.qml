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

    function getContactIcon(name) {
        console.log("getContactIcon", name);
        switch (name.substr(0, 1).toUpperCase()) {
            case "А": return ":Contact-id-1";
            case "Б": return ":Contact-id-2";
            case "В": return ":Contact-id-3";
            case "Г": return ":Contact-id-4";
            case "Д": return ":Contact-id-5";
            case "Е": return ":Contact-id-6";
            case "Ё": return ":Contact-id-7";
            case "Ж": return ":Contact-id-8";
            case "З": return ":Contact-id-9";
            case "И": return ":Contact-id-10";
            case "Й": return ":Contact-id-11";
            case "К": return ":Contact-id-12";
            case "Л": return ":Contact-id-13";
            case "М": return ":Contact-id-14";
            case "Н": return ":Contact-id-15";
            case "О": return ":Contact-id-16";
            case "П": return ":Contact-id-17";
            case "Р": return ":Contact-id-18";
            case "С": return ":Contact-id-19";
            case "Т": return ":Contact-id-20";
            case "У": return ":Contact-id-21";
            case "Ф": return ":Contact-id-22";
            case "Х": return ":Contact-id-23";
            case "Ц": return ":Contact-id-24";
            case "Ч": return ":Contact-id-25";
            case "Ш": return ":Contact-id-26";
            case "Щ": return ":Contact-id-27";
            case "Ъ": return ":Contact-id-28";
            case "Ь": return ":Contact-id-29";
            case "Ы": return ":Contact-id-30";
            case "Э": return ":Contact-id-31";
            case "Ю": return ":Contact-id-32";
            case "Я": return ":Contact-id-33";
            default: return ":Contact-id-noname";
        }

/*
    Connections {
        target: DialerUtils
        function onMuteChanged(muted) {
            muteButton.toggledOn = muted
        }
        function onSpeakerModeChanged(enabled) {
            speakerButton.toggledOn = enabled
        }
    }
*/

    ColumnLayout {
        id: activeCallUi

        anchors {
            fill: parent
        }

        Rectangle {
            id: info
            Layout.fillWidth: true
            height: 1050

            Kirigami.Icon {
                anchors.horizontalCenter: parent.horizontalCenter
                y: 87
                width: 172
                height: 172
                source: getContactIcon(contact.text)
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
                font.pixelSize: {
                    // If contact name is the same as phone number, then remove contact name and make phone number text bigger
                    if (ActiveCallModel.communicationWith === ContactUtils.displayString(ActiveCallModel.communicationWith)) {
                        return 72;
                    } else {
                        return 56;
                    }
                }
                lineHeight: 48
                minimumPixelSize: 56
                maximumLineCount: 3
                text: CallUtils.formatNumber(ActiveCallModel.communicationWith)
                color: "#444444"
                width: 620
                height: 120
                y: 402
                anchors.horizontalCenter: info.horizontalCenter
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

                text: {
                    // If contact name is the same as phone number, then remove contact name and make phone number text bigger
                    if (ActiveCallModel.communicationWith === ContactUtils.displayString(ActiveCallModel.communicationWith)) {
                        return '';
                    } else {
                        return ContactUtils.displayString(phone.text);
                    }
                }
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
/*
            CallPageButton {
                id: muteButton

                width: 296
                height: 164
                x: 50
                y: 75

                //iconSource: ":icons-mic"
                iconSource: ":btn-mic-on"
                text: i18n("Mute")
                visible: false

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
                visible: false
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
                visible: false
                
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
                visible: false
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
*/
            CallPageButton {
                id: endCallButton

                width: 404
                height: 164
                x: 158
                y: 137

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
