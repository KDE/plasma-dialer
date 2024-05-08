// SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Rectangle {
    implicitHeight: Kirigami.Units.gridUnit * 5 + Kirigami.Units.smallSpacing * 3
    color: Kirigami.Theme.backgroundColor

    ToolButton {
        Layout.preferredWidth: Kirigami.Units.gridUnit * 2
        Layout.preferredHeight: Kirigami.Units.gridUnit * 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Kirigami.Units.smallSpacing
        onClicked: {
            Qt.quit();
        }

        Kirigami.Icon {
            source: "window-close-symbolic"
            isMask: true
            smooth: true
            anchors.centerIn: parent
            width: Kirigami.Units.gridUnit * 1.5
            height: width
        }

    }

}
