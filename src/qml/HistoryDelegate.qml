/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.2 as Kirigami
import org.kde.phone.dialer 1.0

Kirigami.AbstractListItem {
    id: root

    highlighted: false
    onClicked: call(model.number)

    RowLayout {
        Kirigami.Icon {
            width: Kirigami.Units.iconSizes.medium
            height: width
            source: {
                switch (model.callType) {
                case DialerUtils.IncomingRejected:
                    return "call-stop";
                case DialerUtils.IncomingAccepted:
                    return "call-incoming";
                case DialerUtils.Outgoing:
                    return "call-outgoing";
                }
            }
        }

        ColumnLayout {
            Controls.Label {
                text: model.displayName
            }
            Controls.Label {
                text: model.number
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Controls.Label {
                Layout.alignment: Qt.AlignRight
                text: Qt.formatTime(model.time, Qt.locale().timeFormat(Locale.ShortFormat));
            }
            Controls.Label {
                Layout.alignment: Qt.AlignRight
                text: i18n("Duration: %1", secondsToTimeString(model.duration));
                visible: model.duration > 0
            }
        }
    }
}
