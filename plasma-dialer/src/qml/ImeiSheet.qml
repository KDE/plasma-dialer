// SPDX-FileCopyrightText: 2021 Smitty van Bodegom <me@smitop.com>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.OverlaySheet {
    id: imeiSheet

    property var imeis: []

    ColumnLayout {
        id: columnLayout

        spacing: Kirigami.Units.largeSpacing * 5
        Layout.fillWidth: true

        Controls.Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.family: "monospace"
            text: imeis.length === 0 ? i18n("No IMEIs found") : imeis.join("\n")
        }

    }

    header: Kirigami.Heading {
        text: i18np("IMEI", "IMEIs", imeiSheet.imeis.length)
    }

}
