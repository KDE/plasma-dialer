// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.0
import QtQuick.Controls 2.7 
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.13 as Kirigami

Kirigami.InlineMessage {
    property var dialerUtils
    type: Kirigami.MessageType.Information
    text: i18n("In call with %1.", dialerUtils.callContactAlias)
    visible: dialerUtils.callState === dialerUtils.Active || dialerUtils.callState === dialerUtils.Dialing
    actions: [
        Kirigami.Action {
            icon.name: "call-start"
            text: i18n("View")
            onTriggered: root.pageStack.layers.push(callPage)
        }
    ]
} 
