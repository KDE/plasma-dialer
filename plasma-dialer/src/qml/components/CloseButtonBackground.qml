// SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Rectangle {
    implicitHeight: Kirigami.Units.gridUnit * 5 + Kirigami.Units.smallSpacing * 3
    color: backgroundColor

    ToolButton {
        Layout.preferredWidth: Kirigami.Units.gridUnit * 2
        Layout.preferredHeight: Kirigami.Units.gridUnit * 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Kirigami.Units.smallSpacing
        Kirigami.Icon {
            source: "window-close-symbolic"
            isMask: true
            smooth: true
            anchors.centerIn: parent
            width: Kirigami.Units.gridUnit * 1.5
            height: width
        }
        onClicked: {
            Qt.quit()
        }
    }
}
