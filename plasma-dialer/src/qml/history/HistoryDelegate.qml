// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.12 as Kirigami
import org.kde.telephony 1.0

Kirigami.AbstractCard {
    id: root
    implicitWidth: 620
    implicitHeight: 132
    onClicked: applicationWindow().call(model.communicationWith)
    
    Rectangle {
        id: container
        parent: root
        anchors.fill: parent
        property var currentAbonent : getContactFromPhonebook(model.communicationWith)
                

         Rectangle {
                id: historyPersonIconContainer
                parent: container
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin : 20
                anchors.topMargin : 20
                width: 93
                height: 93
                radius: 5.23
                
                Kirigami.Icon {
                    parent: historyPersonIconContainer
                    anchors.fill: parent
                    color: "white"
                    source: {
                        if (container.currentAbonent === "Неизвестный номер"){
                            return ":Contact-id-noname"
                        } else {
                            return getContactIcon(container.currentAbonent)
                        }
                    }   
                }
                
        }

        ColumnLayout {
            id: callerinfoContainer
            anchors.left: historyPersonIconContainer.right
            anchors.top: container.top
            anchors.leftMargin: 20
            anchors.topMargin: 20

            Controls.Label {
                id: callContactDisplayLabel
                text: getContactFromPhonebook(model.communicationWith)
                font.pixelSize: 26
                font.family: "Manrope"
            }

            Controls.Label {
                id: numberLabel
                color: "#C4C4C4"
                font.family: "Manrope"
                text: model.communicationWith
                font.pixelSize: 26
            }
        }

        ColumnLayout {
           id: callinfoContainer
           anchors.right: container.right
           anchors.top: container.top

           anchors.topMargin : 30
           anchors.rightMargin: 30

            Kirigami.Icon {
                id: inOrOutCallIcon
                width: 37.12
                height: 37
                source: {
                    if (model.direction == DialerTypes.CallDirection.Incoming) {
                        return ":icon-call-missed"
                    } else if (model.direction == DialerTypes.CallDirection.Outgoing) {
                        return ":icon-call-received";
                    } else {
                        return ":icon-call-missed";
                    }
                }
            }

            Controls.Label {
                text: secondsToTimeString(model.duration)
                color: "#C4C4C4"
                font.family: "Manrope"
                font.pixelSize: 20
            }
        }
    }
}

