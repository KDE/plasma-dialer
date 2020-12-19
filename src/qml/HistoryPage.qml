/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

import org.kde.phone.dialer 1.0

Kirigami.ScrollablePage {
    title: i18n("Call History")
    icon.name: "clock"
    
    header: InCallInlineMessage {
        id: inCall
        dialerUtils: DialerUtils
    }
    
    actions.main: Kirigami.Action {
        onTriggered: historyModel.clear()
        text: i18n("Clear history")
        icon.name: "edit-clear-history"
    }

    function secondsToTimeString(seconds) {
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds - (h * 3600)) / 60);
        var s = seconds - h * 3600 - m * 60;
        if(h < 10) h = '0' + h;
        if(m < 10) m = '0' + m;
        if(s < 10) s = '0' + s;
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
        model: historyModel
        section {
            property: "date"
            delegate: Kirigami.ListSectionHeader {
                label: Qt.formatDate(section, Qt.locale().dateFormat(Locale.LongFormat));
            }
        }
        delegate: HistoryDelegate {}
    }
}
