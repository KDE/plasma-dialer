// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.12
import QtQml.Models 2.15

import ".."

import org.kde.kirigami 2.13 as Kirigami
import org.kde.telephony 1.0

Item {

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumHeight: parent.height / 2

    ListView {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing

        model: ActiveCallModel
        delegate: Kirigami.AbstractListItem {
            RowLayout {
                Label {
                    text: communicationWith
                }
            }
        }
    }
}
