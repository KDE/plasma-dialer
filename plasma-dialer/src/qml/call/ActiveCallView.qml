// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models
import org.kde.kirigami as Kirigami
import org.kde.telephony
import ".."

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumHeight: parent.height / 2

    ListView {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing
        model: ActiveCallModel

        delegate: ItemDelegate {
            text: communicationWith
            width: ListView.view.width
        }

    }

}
