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
                    color: "#FFFFFF"
                    parent: callingLabel
                    anchors.bottom: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 20
                    width: 172
                    height: 172
                    source: ":Contact-id-noname"
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
                color: "#999999"
                text: "8-800-555-35-35"
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
                color: "#999999"
                text: "Неизвестный номер"

                Component.onCompleted: {
                    contactsSearch.setFilterFixedString(numberLabel.text);
                    if (contactsSearch.rowCount() > 0)
                        aliasLabel.text = contactsSearch.data(contactsSearch.index(0, 0));
                }
            }
        } //somebody calls label

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
            enabled: statusLabel.text.length > 0
            onClicked: {
                CallUtils.accept(activeDeviceUni(), activeCallUni())
                switchToPage(getPage("Call"), 0)
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
            enabled: statusLabel.text.length > 0
            onClicked: {
                CallUtils.hangUp(activeDeviceUni(), activeCallUni());
                Qt.quit()
            } 
        }
    }
    } 
    }
} //page
