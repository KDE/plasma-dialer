// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.19 as Kirigami

import org.kde.telephony 1.0

import "call"
import "history"

Kirigami.ScrollablePage {
    title: i18n("Call History")
    icon.name: "clock"
    
    header: ColumnLayout {
        anchors.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.smallSpacing
        InCallInlineMessage {}
    }
    
    actions.main: Kirigami.Action {
        onTriggered: promptDialog.open()
        text: i18n("Clear history")
        icon.name: "edit-clear-history"
    }
    
    actions.contextualActions: [
        Kirigami.Action {
            displayHint: Kirigami.Action.IconOnly
            visible: !applicationWindow().isWidescreen
            iconName: "settings-configure"
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
        if(h === 0) return '' + m + ':' + s;
        return '' + h + ':' + m + ':' + s;
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        text: i18n("No recent calls")
        icon.name: "call-outgoing"
        visible: view.count == 0
    }


    ListView {
        id: view
        model: CallHistoryModel
        section {
            property: "date"
            delegate: Kirigami.ListSectionHeader {
                label: Qt.formatDate(section, Qt.locale().dateFormat(Locale.LongFormat));
            }
        }
        delegate: HistoryDelegate {}
    }
    
    Kirigami.PromptDialog {
        id: promptDialog
        title: "Clear history?"
        subtitle: "Delete the call history log without option to restore it"
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
        onAccepted: CallHistoryModel.clear()
        onRejected: close()
    }
}
