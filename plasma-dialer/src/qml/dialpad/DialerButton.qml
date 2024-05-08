// SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
// SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Item {
    id: root

    property string text
    property string sub
    property string display
    property string subdisplay
    property bool voicemail: false
    property bool special: false

    signal clicked(string text)
    signal held(string text)

    Layout.fillWidth: true
    Layout.fillHeight: true

    Rectangle {
        anchors.fill: parent
        z: -1
        color: Kirigami.Theme.highlightColor
        radius: Kirigami.Units.smallSpacing
        opacity: mouse.pressed ? 0.4 : 0
    }

    Controls.AbstractButton {
        id: mouse

        anchors.fill: parent
        onClicked: root.clicked(parent.text)
        onPressAndHold: root.held(parent.text)
    }

    ColumnLayout {
        spacing: -5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Controls.Label {
            id: main

            font.pixelSize: applicationWindow().smallMode ? Kirigami.Units.gridUnit * 1.2 : Kirigami.Units.gridUnit * 1.75
            text: root.display || root.text
            opacity: special ? 0.4 : 1
            Layout.minimumWidth: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.weight: Font.Light
        }

        Controls.Label {
            id: longHold

            text: root.subdisplay || root.sub
            opacity: 0.4
            Layout.minimumWidth: parent.width
            horizontalAlignment: Text.AlignHCenter

            Kirigami.Icon {
                anchors.horizontalCenter: parent.horizontalCenter
                source: "call-voicemail"
                visible: root.voicemail
                width: Kirigami.Units.iconSizes.small
                height: width
                color: Kirigami.Theme.textColor
            }

        }

    }

}
