// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls 
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.telephony

Kirigami.InlineMessage {
    type: Kirigami.MessageType.Information
    visible: ActiveCallModel.active
    text: i18n("Active call list")
    actions: [
        Kirigami.Action {
            icon.name: "call-start"
            text: i18n("View")
            onTriggered: applicationWindow().pageStack.layers.push(getPage("Call"))
        }
    ]
    Layout.fillWidth: true
    Layout.leftMargin: Kirigami.Units.smallSpacing
    Layout.rightMargin: Kirigami.Units.smallSpacing
}
