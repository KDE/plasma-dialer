// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.2 as Kirigami
import org.kde.telephony 1.0

Kirigami.AbstractListItem {
    id: root

    highlighted: false
    onClicked: applicationWindow().call(model.communicationWith)

    RowLayout {
        Kirigami.Icon {
            width: Kirigami.Units.iconSizes.medium
            height: width
            source: {
                if (model.direction == DialerTypes.CallDirection.Incoming) {
                    if (model.stateReason == DialerTypes.CallStateReason.Accepted) {
                        return "call-incoming"
                    } else {
                        return "call-stop"
                    }
                } else if (model.direction == DialerTypes.CallDirection.Outgoing) {
                    return "call-outgoing";
                } else {
                    return "call-start";
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Controls.Label {
                id: callContactDisplayLabel
                font.family: "Manrope"
                text: model.communicationWith
                Layout.fillWidth: true
            }
            Controls.Label {
                id: numberLabel
                visible: callContactDisplayLabel.text !== text
                font.family: "Manrope"
                text: ContactUtils.displayString(model.communicationWith)
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Controls.Label {
                Layout.alignment: Qt.AlignRight
                font.family: "Manrope"
                text: Qt.formatDateTime(model.startedAt, Qt.locale().dateTimeFormat(Locale.ShortFormat));
            }
            Controls.Label {
                Layout.alignment: Qt.AlignRight
                font.family: "Manrope"
                text: i18n("Duration: %1", secondsToTimeString(model.duration));
                visible: model.duration > 0
            }
        }
    }
}
