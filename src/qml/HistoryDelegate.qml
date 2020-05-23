/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls

import org.kde.kirigami 2.2 as Kirigami
import org.kde.phone.dialer 1.0

Kirigami.AbstractListItem {
    id: root

    highlighted: false

    RowLayout {
        Kirigami.Icon {
            width: Kirigami.Units.iconSizes.medium
            height: width
            source: {
                switch (model.callType) {
                case DialerUtils.IncomingRejected:
                    return "list-remove";
                case DialerUtils.IncomingAccepted:
                    return "go-down";
                case DialerUtils.Outgoing:
                    return "go-up";
                }
            }
        }

        ColumnLayout {
            Controls.Label {
                text: "Name (todo)"
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

        MouseArea {
            anchors.fill: parent
            onClicked: call(model.number);
            drag.axis: Drag.XAxis
            drag.target: root
            onReleased: {
                if (drag.active) {
                    if (root.x > root.width / 3 || root.x < root / -3) {
                        removeAnim.running = true;
                    } else {
                        resetAnim.running = true;
                    }
                }
            }
        }

        SequentialAnimation {
            id: removeAnim
            XAnimator {
                target: root
                from: root.x
                to: root.x > 0 ? width : -width
                duration: units.longDuration
                easing.type: Easing.InOutQuad
            }
            ScriptAction {
                script: historyModel.remove(model.index)
            }
        }

        XAnimator {
            id: resetAnim
            target: root
            from: root.x
            to: 0
            duration: units.longDuration
            easing.type: Easing.InOutQuad
        }
    }
}
