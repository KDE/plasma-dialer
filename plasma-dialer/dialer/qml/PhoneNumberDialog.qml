/*
 *   SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Kirigami.OverlaySheet {
    id: root

    property alias numbers: list.model
    property alias title: heading.text

    signal numberSelected(string number)

    ListView {
        id: list

        implicitWidth: Kirigami.Units.gridUnit * 20
        model: 4

        delegate: ItemDelegate {
            id: delegate

            text: modelData.typeLabel
            width: ListView.view.width
            onClicked: {
                close();
                root.numberSelected(modelData.normalizedNumber);
            }

            contentItem: Kirigami.TitleSubtitle {
                title: delegate.text
                subtitle: modelData.number
            }

        }

    }

    header: Kirigami.Heading {
        id: heading
    }

}
