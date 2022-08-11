// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.5 as Controls
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.telephony 1.0

import "call"
import "history"



Kirigami.ScrollablePage {

    //header for search panel
    /*
    header: ColumnLayout {
        Rectangle{
            width: 720
            height: 96
            //color: "black"
        }
    }
    */
    footer: Rectangle {
            id:footerRectangle 
            width: 720
            height:187
            color: "#437431"
            
                Item {
                    id: recentButton
                    parent: footerRectangle
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.topMargin: 30
                    anchors.leftMargin: 70
                    implicitHeight: 150
                    implicitWidth: 220

                    Kirigami.Icon {
                        id: recentIcon
                        anchors.fill: parent
                        source: ":btn-recgr"
                    }

                }

                Item {
                    id: contactsButton
                    parent: footerRectangle
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 30
                    anchors.rightMargin: 70
                    implicitHeight: 150
                    implicitWidth: 220

                    Kirigami.Icon {
                        id: contactsIcon
                        source: ":btn-contrecent-notpressed"
                        anchors.fill: parent
                        color: "white"
                    }

                    Controls.AbstractButton {
                        id: contactsAbstractButton
                        anchors.fill: parent
                        onClicked: {
                            applicationWindow().switchToPage(applicationWindow().getPage("Contacts"), 0);
                        } 
                    }
                }
            }
    
    Rectangle {
        parent: footerRectangle
        color:"transparent"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 210
        anchors.right: parent.right
        anchors.rightMargin: 20
        width: 187
        height: 164
        
        
        Kirigami.Icon{
            anchors.fill: parent
            source: toDialerButton.pressed ? ":btn-dialercontact-all-pressed" : ":btn-dialercontact-all"
        }

        Controls.AbstractButton{
            id: toDialerButton
            anchors.fill: parent
            onClicked: {
                switchToPage(getPage("Dialer"), 0)
            }
        }
  
    }

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
        if(h === 0) return '' + m + ':' + s;
        return '' + h + ':' + m + ':' + s;
    }

    function getContactFromPhonebook(adressee) {
        var contactNameFromBook = ContactUtils.displayString(adressee)
        
        console.log(contactNameFromBook)
        return contactNameFromBook === adressee ? "Неизвестный номер" : contactNameFromBook

    }
    
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        text: "Нет истории звонков"
        icon.name: "call-outgoing"
        visible: view.count == 0
    }
    

    Kirigami.CardsListView {
        id: view
        model: CallHistoryModel
        /*
        section {
            property: "date"
            delegate: Kirigami.ListSectionHeader {
                label: Qt.formatDate(section, Qt.locale().dateFormat(Locale.LongFormat));
            }
        }
        */
        delegate: HistoryDelegate {}
    }
}
