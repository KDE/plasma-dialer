/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.telephony
import org.kde.plasma.dialer
import org.kde.plasma.dialer.shared

Kirigami.Page {
    id: dialerPage

    property alias numberEntryText: statusLabel.text
    property alias pad: dialPad
    // page animation
    property real yTranslate: 0

    title: i18n("Dialer")
    icon.name: "call-start"
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    actions: [
        Kirigami.Action {
            id: settingsAction

            icon.name: "settings-configure"
            displayHint: Kirigami.DisplayHint.IconOnly
            visible: !applicationWindow().isWidescreen
            enabled: !applicationWindow().lockscreenMode
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    ColumnLayout {
        id: dialPadArea

        anchors.fill: parent
        spacing: 0

        QQC2.ScrollView {
            id: scrollView

            contentWidth: -1 // no horizontal scrolling necessary
            Layout.minimumWidth: dialerPage.width
            Layout.maximumWidth: dialerPage.width
            Layout.preferredHeight: applicationWindow().smallMode ? implicitHeight : parent.height * 0.3

            QQC2.Label {
                id: statusLabel

                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: implicitHeight > scrollView.height ? Qt.AlignTop : Qt.AlignVCenter
                width: dialerPage.width
                height: scrollView.height
                font.pixelSize: applicationWindow().smallMode ? Kirigami.Units.gridUnit * 1.6 : Kirigami.Units.gridUnit * 2.3
                font.weight: Font.Light
                wrapMode: QQC2.Label.WrapAnywhere
                text: CallUtils.formatNumber(dialPad.number)
            }

        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor

            Dialpad {
                id: dialPad

                anchors.fill: parent
                anchors.bottomMargin: Kirigami.Units.largeSpacing * 2
                anchors.topMargin: Kirigami.Units.largeSpacing * 2
                anchors.leftMargin: Kirigami.Units.largeSpacing * 3
                anchors.rightMargin: Kirigami.Units.largeSpacing * 3
                focus: true
            }

        }

        transform: Translate {
            y: yTranslate
        }

    }

    header: ColumnLayout {
        spacing: 0

        Kirigami.InlineMessage {
            id: daemonsError

            type: Kirigami.MessageType.Error
            position: Kirigami.InlineMessage.Header
            text: i18n("Telephony daemons are not responding")
            visible: !DialerUtils.isValid
            Layout.fillWidth: true
        }

        Kirigami.InlineMessage {
            id: devicesError

            type: Kirigami.MessageType.Error
            position: Kirigami.InlineMessage.Header
            text: i18n("Modem devices are not found")
            visible: DeviceUtils.deviceUniList.length < 1
            Layout.fillWidth: true
        }

        Kirigami.InlineMessage {
            id: voicemailError

            type: Kirigami.MessageType.Error
            position: Kirigami.InlineMessage.Header
            text: i18n("Voicemail number couldn't be found")
            visible: dialPad.voicemailFail
            Layout.fillWidth: true
        }

        InCallInlineMessage {
            position: Kirigami.InlineMessage.Header
            Layout.fillWidth: true
        }

    }

}
