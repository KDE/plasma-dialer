// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import plasmadialerfakeserver as Fakeserver

Kirigami.ApplicationWindow {
    id: root

    width: 700
    height: 600
    visibility: "Windowed"

    title: "Test Dialer"

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    property int currentTab: 0

    Component.onCompleted: switchToPage(0)

    function switchToPage(tabIndex) {
        currentTab = tabIndex
        while (pageStack.depth > 0) pageStack.pop()
        pageStack.push(tabIndex === 0 ? callPage : ussdPage)
    }

    footer: Kirigami.NavigationTabBar {
        actions: [
            Kirigami.Action {
                text: "Calls"
                icon.name: "call-start"
                checked: root.currentTab === 0
                onTriggered: switchToPage(0)
            },
            Kirigami.Action {
                text: "USSD"
                icon.name: "dialog-messages"
                checked: root.currentTab === 1
                onTriggered: switchToPage(1)
            }
        ]
    }

    Component {
        id: callPage

        Kirigami.ScrollablePage {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            title: "Calls"

            actions: [
                Kirigami.Action {
                    text: "Start incoming call"
                    icon.name: 'list-add'
                    onTriggered: Fakeserver.Server.startIncomingCall()
                }
            ]

            ListView {
                id: listView
                model: Fakeserver.Server.modemMocker.Calls
                spacing: Kirigami.Units.gridUnit

                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    visible: listView.count === 0
                    icon.name: "call-start"
                    text: "No active calls"
                }

                delegate: QQC2.Control {
                    width: listView.width
                    topPadding: Kirigami.Units.gridUnit
                    bottomPadding: Kirigami.Units.gridUnit
                    leftPadding: Kirigami.Units.gridUnit
                    rightPadding: Kirigami.Units.gridUnit

                    contentItem: Kirigami.Card {
                        Layout.fillWidth: true
                        actions: [
                            Kirigami.Action {
                                text: "Hang Up"
                                icon.name: "call-stop"
                                onTriggered: Fakeserver.Server.stopIncomingCall(modelData)
                            }
                        ]

                        contentItem: ColumnLayout {
                            Kirigami.Heading {
                                Layout.margins: Kirigami.Units.largeSpacing
                                Layout.fillWidth: true
                                text: modelData
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: ussdPage

        Kirigami.ScrollablePage {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            title: "USSD"

            ColumnLayout {
                spacing: Kirigami.Units.largeSpacing

                Kirigami.Heading {
                    text: "Send network-initiated USSD"
                    level: 3
                }

                QQC2.TextField {
                    id: ussdMessageField
                    Layout.fillWidth: true
                    placeholderText: "USSD message text..."
                    text: "Your balance is $42.00. Reply 1 for details."
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.smallSpacing

                    QQC2.Button {
                        text: "Send Notification"
                        icon.name: "dialog-information"
                        QQC2.ToolTip.text: "Sends a one-way network notification (no response expected)"
                        QQC2.ToolTip.visible: hovered
                        onClicked: {
                            if (ussdMessageField.text !== "") {
                                Fakeserver.Server.sendUssdNotification(ussdMessageField.text)
                            }
                        }
                    }

                    QQC2.Button {
                        text: "Send Request"
                        icon.name: "dialog-question"
                        QQC2.ToolTip.text: "Sends a network request that expects a user response"
                        QQC2.ToolTip.visible: hovered
                        onClicked: {
                            if (ussdMessageField.text !== "") {
                                Fakeserver.Server.sendUssdRequest(ussdMessageField.text)
                            }
                        }
                    }
                }
            }
        }
    }
}
