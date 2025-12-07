// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami as Kirigami
import org.kde.telephony
import org.kde.kirigamiaddons.components as Addons
import org.kde.people as KPeople

import ".."

Item {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.minimumHeight: parent.height / 2

    property string contactUri: ContactUtils.phoneNumberToContactUri(ActiveCallModel.communicationWith);

    KPeople.PersonData {
        id: personData
        personUri: root.contactUri
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing

        // phone number/alias
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2
            text: contactUri !== "" ? personData.person.name : ContactUtils.displayString(ActiveCallModel.communicationWith)
            font.weight: Font.Light
        }

        // time spent on call
        Controls.Label {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.bold: true
            text: {
                if (ActiveCallModel.inCall)
                    return secondsToTimeString(ActiveCallModel.duration);

                if (ActiveCallModel.incoming)
                    return "Incoming...";

                if (ActiveCallModel.active)
                    return "Calling...";

                return '';
            }
            visible: text != ""
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Addons.Avatar {
                anchors.centerIn: parent
                property real length: Math.min(Kirigami.Units.gridUnit * 10,
                                                Math.min(parent.width - Kirigami.Units.gridUnit * 2, parent.height - Kirigami.Units.gridUnit * 2))
                width: length
                height: length

                source: contactUri !== "" ? ContactUtils.photoImageProviderUri(root.contactUri) : ""
                name: contactUri !== "" ? personData.person.name : ""
                imageMode: Addons.Avatar.ImageMode.AdaptiveImageOrInitals
            }
        }
    }
}
