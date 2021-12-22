/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ScrollablePage {
    id: root
    
    title: i18n("Settings")
    icon.name: "settings-configure"
    
    Kirigami.ColumnView.fillWidth: false
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    
    // settings list
    ColumnLayout {
        spacing: 0

        Kirigami.FormLayout {
            id: form
            Layout.fillWidth: true
            Layout.maximumWidth: root.width - root.leftPadding - root.rightPadding
            wideMode: false
            
            Button {
                Kirigami.FormData.label: i18n("More Info:")
                text: i18n("About")
                icon.name: "help-about-symbolic"
                onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("About"))
            }
        }
    }
}
