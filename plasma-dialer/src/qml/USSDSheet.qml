// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: ussdSheet

    property bool isError: false
    property string messageText: ""
    property bool replyRequested: false

    signal responseReady(string response)
    signal cancelSessionRequested()

    function showNotification(text) {
        isError = false;
        ussdSheet.messageText = text;
    }

    function changeState(state) {
        ussdSheet.replyRequested = (state == "user-response");
    }

    function showError(errorMessage) {
        isError = true;
        ussdSheet.messageText = errorMessage;
    }

    onVisibleChanged: {
        if (!visible) {
            ussdSheet.messageText = "";
        }
    }

    title: isError ? i18n("USSD Error") : i18n("USSD Message")
    padding: Kirigami.Units.largeSpacing

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Controls.BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            width: Kirigami.Units.gridUnit * 2
            height: width
            visible: (ussdSheet.messageText === "")
        }

        Controls.Label {
            text: ussdSheet.messageText
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            visible: (ussdSheet.messageText !== "")
        }

        Loader {
            Layout.fillWidth: true
            sourceComponent: ussdSheet.isError ? cancelButtonComponent : responseFieldComponent

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
                                ussdSheet.responseReady(responseField.text);
                                responseField.text = "";
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
                        ussdSheet.cancelSessionRequested();
                        ussdSheet.close();
                    }
                }

            }
        }
    }
}
