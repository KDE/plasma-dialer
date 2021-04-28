// SPDX-FileCopyrightText: 2021 Smitty van Bodegom <me@smitop.com>
//
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.0
import QtQuick.Controls 2.7 as Controls
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

Kirigami.OverlaySheet {

    id: imeiSheet

    property var imeis: []

    header: Kirigami.Heading {
        text: i18np("IMEI", "IMEIs", imeiSheet.imeis.length)
    }

    ColumnLayout {
        id: columnLayout
        spacing: Kirigami.Units.largeSpacing * 5
        Layout.fillWidth: true

        Controls.Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.family: "monospace"
            text: imeiSheet.imeis.join("\n")
        }
    }
}
