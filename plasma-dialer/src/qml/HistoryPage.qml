// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.telephony
import "call"
import "history"

Kirigami.ScrollablePage {
    id: historyPage

    // page animation
    property real yTranslate: 0

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if (m < 10)
            m = '0' + m;

        if (s < 10)
            s = '0' + s;

        if (h === 0)
            return '' + m + ':' + s;

        return '' + h + ':' + m + ':' + s;
    }

    title: i18n("Call History")
    icon.name: "clock"
    actions: [
        Kirigami.Action {
            icon.name: "edit-clear-history"
            onTriggered: promptDialog.open()
            text: i18n("Clear history")
        },
        Kirigami.Action {
            icon.name: "settings-configure"
            displayHint: Kirigami.DisplayHint.IconOnly
            visible: !applicationWindow().isWidescreen
            enabled: !applicationWindow().lockscreenMode
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    ListView {
        id: view

        model: CallHistoryModel

        section {
            property: "date"

            delegate: Kirigami.ListSectionHeader {
                label: Qt.formatDate(section, Qt.locale().dateFormat(Locale.LongFormat))
            }

        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No recent calls")
            icon.name: "call-outgoing"
            visible: view.count == 0
        }

        transform: Translate {
            y: yTranslate
        }

        delegate: HistoryDelegate {
            width: ListView.view.width
        }

    }

    Kirigami.PromptDialog {
        id: promptDialog

        title: "Clear history?"
        subtitle: "Delete the call history log without option to restore it"
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
        onAccepted: CallHistoryModel.clear()
        onRejected: close()
        maximumHeight: applicationWindow().height
    }

    header: ColumnLayout {
        spacing: 0
        InCallInlineMessage {
            position: Kirigami.InlineMessage.Header
            Layout.fillWidth: true
        }
    }

}
