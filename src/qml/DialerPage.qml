/*
 *   SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
 *   SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.9 as Kirigami
import "Dialpad"
import org.kde.phone.dialer 1.0

Kirigami.Page {
    id: dialer

    property alias numberEntryText: statusLabel.text
    property alias pad: dialPad

    title: i18n("Dialer")
    icon.name: "call-start"

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    header: ColumnLayout {
        anchors.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.smallSpacing
        Kirigami.InlineMessage {
            id: callError
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            type: Kirigami.MessageType.Error
            text: i18n("Unable to make a call at this moment")
            visible: DialerUtils.callState === DialerUtils.Failed
        }
        Kirigami.InlineMessage {
            id: voicemailError
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            type: Kirigami.MessageType.Error
            text: i18n("Voicemail number couldn't be found")
            visible: dialPad.voicemailFail
        }
        InCallInlineMessage {
            id: inCall
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            dialerUtils: DialerUtils
        }
    }

    ColumnLayout {
        id: dialPadArea
        anchors.fill: parent
        spacing: 0

        QQC2.ScrollView {
            id: scrollView
            contentWidth: -1 // no horizontal scrolling necessary
            Layout.minimumWidth: dialer.width
            Layout.maximumWidth: dialer.width
            Layout.preferredHeight: applicationWindow().smallMode ? implicitHeight : parent.height * 0.3
            
            QQC2.Label {
                id: statusLabel

                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: implicitHeight > scrollView.height ? Qt.AlignTop : Qt.AlignVCenter
                width: dialer.width
                height: scrollView.height
                font.pixelSize: applicationWindow().smallMode ? Kirigami.Units.gridUnit * 1.6 : Kirigami.Units.gridUnit * 2.3
                font.weight: Font.Light
                wrapMode: QQC2.Label.WrapAnywhere

                text: DialerUtils.formatNumber(dialPad.number)
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
    }
}
