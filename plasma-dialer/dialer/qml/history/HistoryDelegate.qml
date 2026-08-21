// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.telephony

Controls.ItemDelegate {
    id: root

    highlighted: false
    onClicked: applicationWindow().call(model.communicationWith)

    contentItem: RowLayout {
        Kirigami.Icon {
            width: Kirigami.Units.iconSizes.medium
            height: width
            color: source == "call-incoming" ? "blue" :
                    source == "call-missed" ? "red" :
                    source == "call-outgoing" ? "green" : Kirigami.Theme.textColor
            source: {
                if (model.direction == DialerTypes.CallDirection.Incoming) {
                    if (model.duration > 0)
                        return "call-incoming";
                    else
                        return "call-missed";
                } else {
                    if (model.direction == DialerTypes.CallDirection.Outgoing)
                        return "call-outgoing";
                    else
                        return "call-start";
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true

            Controls.Label {
                id: callContactDisplayLabel

                text: model.communicationWith
                Layout.fillWidth: true
            }

            Controls.Label {
                id: numberLabel

                visible: callContactDisplayLabel.text !== text
                text: ContactUtils.displayString(model.communicationWith)
                Layout.fillWidth: true
            }

        }

        ColumnLayout {
            Layout.fillWidth: true

            Controls.Label {
                Layout.alignment: Qt.AlignRight
                text: Qt.formatDateTime(model.startedAt, Qt.locale().dateTimeFormat(Locale.ShortFormat))
            }

            Controls.Label {
                Layout.alignment: Qt.AlignRight
                text: i18n("Duration: %1", secondsToTimeString(model.duration))
                visible: model.duration > 0
            }

        }

    }

}
