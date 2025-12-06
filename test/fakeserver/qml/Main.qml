// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import plasmadialerfakeserver as Fakeserver

Kirigami.ApplicationWindow {
    id: root

    width: 700
    height: 600
    visibility: "Windowed"

    title: "Test Dialer"

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    pageStack.initialPage: Kirigami.ScrollablePage {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        title: "Call List"

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
