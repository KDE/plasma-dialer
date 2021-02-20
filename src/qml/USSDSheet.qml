// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.0
import QtQuick.Controls 2.7 as Controls
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

Kirigami.OverlaySheet {

    id: ussdSheet

    property string notificationText
    property bool replyRequested: false

    signal responseReady(string response)

    function showNotification(text, requestReply = false) {
        ussdSheet.replyRequested = requestReply
        ussdSheet.notificationText = text
    }

    onSheetOpenChanged: {
        if (!sheetOpen) {
            ussdSheet.notificationText = ""
        }
    }

    header: Kirigami.Heading {
        text: i18n("USSD Message")
    }
    footer: Kirigami.ActionTextField {
        id: responseField
        visible: ussdSheet.replyRequested
        placeholderText: i18n("Write response...")
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

    Controls.BusyIndicator {
        anchors.centerIn: columnLayout
        width: Kirigami.Units.gridUnit * 2
        height: width
        visible: (ussdSheet.notificationText === "") && sheetOpen
    }

    ColumnLayout {
        id: columnLayout
        spacing: Kirigami.Units.largeSpacing * 5
        Layout.fillWidth: true

        Controls.Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: ussdSheet.notificationText
        }
    }
}
