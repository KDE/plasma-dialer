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
import org.kde.people 1.0 as KPeople

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

    KPeople.PersonsSortFilterProxyModel {
        id: contactsSearch
        sourceModel: KPeople.PersonsModel {
        }
        filterRole: KPeople.PersonsModel.PhoneNumberRole
        sortRole: Qt.DisplayRole
        filterCaseSensitivity: Qt.CaseInsensitive
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
    }

    Rectangle {
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

            // somebody calls label
            Controls.Label {
                id: callingLabel
                parent: topSide
                anchors.centerIn: topSide
                font.family: "Manrope"
                font.pixelSize: 26
                color: "#999999"
                text: "Вызов от..."

                //icon
                Rectangle {
                    id: personIcon
                    parent: callingLabel
                    anchors.bottom: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 20
                    color: "#999999"
                    width: 172
                    height: 172
                    radius: 10

                    Kirigami.Icon {
                        parent: callingLabel
                        anchors.bottom: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 20
                        width: 172
                        height: 172
                        source: getContactIcon(ContactUtils.displayString(ActiveCallModel.communicationWith))
                    }
                }

                // phone number
                Controls.Label {
                    id: numberLabel
                    parent: callingLabel
                    anchors.top: parent.bottom
                    anchors.topMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: "Manrope"
                    font.pixelSize: 56
                    text: ActiveCallModel.communicationWith
                }

                // alias
                Controls.Label {
                    id: aliasLabel
                    parent: callingLabel
                    anchors.top: numberLabel.bottom
                    anchors.topMargin: 170
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: "Manrope"
                    font.pixelSize: 40
                    text: {
                        // If contact name is the same as phone number, then display "Unknown number"
                        if (ActiveCallModel.communicationWith === ContactUtils.displayString(ActiveCallModel.communicationWith)) {
                            return "Неизвестный номер";
                        } else {
                            return ContactUtils.displayString(ActiveCallModel.communicationWith);
                        }
                    }
                }
            } //somebody calls label
        }

        Rectangle {
            id: bottomSide
            parent: mainRect
            color: "#437431"
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            height: parent.height / 2
            width: parent.width

            //accept call button
            Item {
                id: acceptCallButton
                parent: bottomSide
                anchors.centerIn: parent
                height: 164
                width: 404

                Kirigami.Icon {
                    id: acceptIcon
                    source: acceptAbatractButton.pressed ? ":btn-callgr" : ":btn-call"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    width: 404
                    height: 164
                }

                Controls.AbstractButton {
                    id: acceptAbatractButton
                    anchors.fill: parent
                    enabled: true
                    onClicked: {
                        CallUtils.accept(activeDeviceUni(), activeCallUni());
                    }
                }
            }

            //decline call button
            Item {
                id: declineCallButton
                anchors.top: acceptCallButton.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 5
                height: 164
                width: 404

                Kirigami.Icon {
                    id: declineIcon
                    source: declineAbstractButton.pressed ? ":btn-call-hangupgr" : ":btn-call-hangup"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    width: 404
                    height: 164
                }

                Controls.AbstractButton {
                    id: declineAbstractButton
                    anchors.fill: parent
                    enabled: true
                    onClicked: {
                        CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                    }
                }
            }
        }
    }
} //page
