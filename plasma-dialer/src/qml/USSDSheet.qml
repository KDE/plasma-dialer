// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Kirigami.OverlaySheet {

    id: ussdSheet

    property bool isError: false
    property string messageText: ""
    property bool replyRequested: false

    signal responseReady(string response)
    signal cancelSessionRequested()

    function showNotification(text) {
        isError = false
        ussdSheet.messageText = text
    }

    function changeState(state) {
        ussdSheet.replyRequested = (state=="user-response")
        
        if (ussdSheet.replyRequested) {
            responseField.setFocus()          
        }
    }

    function showError(errorMessage) {
        isError = true
        ussdSheet.messageText = errorMessage
    }

    function onSheetOpenChanged() {
        if (!ussdSheet.sheetOpen) {
            ussdSheet.messageText = ""
        }
    }

    header: Kirigami.Heading {
        text: isError ? i18n("USSD Error") : i18n("USSD Message")
    }

    footer: Loader {
        Component {
            id: responseFieldComponent
            Kirigami.ActionTextField {
                id: responseField
                visible: ussdSheet.replyRequested
                placeholderText: i18n("Write response...")
                inputMethodHints: Qt.ImhPreferNumbers
                onAccepted: text !== "" && sendAction.triggered()
                rightActions: [
                    Kirigami.Action {
                        id: sendAction
                        text: i18n("Send")
                        icon.name: "document-send"
                        enabled: responseField.text !== ""
                        onTriggered: {
                            ussdSheet.responseReady(responseField.text)
                            responseField.text = ""
                        }
                    }
                ]
            }
        }

        Component {
            id: cancelButtonComponent
            Controls.Button {
                icon.name: "dialog-close"
                text: i18n("Cancel USSD session")
                onClicked: {
                    ussdSheet.cancelSessionRequested()
                    ussdSheet.close()
                }
            }
        }

        sourceComponent: ussdSheet.isError ? cancelButtonComponent : responseFieldComponent
    }

    Controls.BusyIndicator {
        anchors.centerIn: columnLayout
        width: Kirigami.Units.gridUnit * 2
        height: width
        visible: (ussdSheet.messageText === "")
    }

    ColumnLayout {
        id: columnLayout
        spacing: Kirigami.Units.largeSpacing * 5
        Layout.fillWidth: true

        Controls.Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: ussdSheet.messageText
        }
    }
}
