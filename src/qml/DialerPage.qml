/*
 *   Copyright 2014 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2014 Marco Martin <mart@kde.org>
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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.9 as Kirigami
import "Dialpad"
import org.kde.phone.dialer 1.0

Kirigami.Page {
    id: dialer

    property alias numberEntryText: status.text

    title: i18n("Dialer")
    icon.name: "call-start"

    header: Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        text: i18n("Unable to make a call at this moment")
        visible: dialerUtils.callState === DialerUtils.Failed
    }

    ColumnLayout {
        id: dialPadArea
        anchors.fill: parent

        QQC2.Label {
            id: status

            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignBottom

            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumHeight: Layout.minimumHeight
            font.pixelSize: units.gridUnit * 2.3

            text: dialerUtils.formatNumber(dialPad.number)
        }

        Dialpad {
            id: dialPad
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
