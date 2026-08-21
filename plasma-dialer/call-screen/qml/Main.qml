// SPDX-FileCopyrightText: 2026 Micah Stanley <stanleymicah@proton.me>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.LocalStorage

import org.kde.kirigami as Kirigami

import org.kde.telephony
import org.kde.plasma.callscreen

import "call"

Kirigami.ApplicationWindow {
    id: root

    property bool lockscreenMode: LockScreenUtils.lockscreenActive
    property bool isWidescreen: !LockScreenUtils.lockscreenActive && root.width >= root.height

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.None
    visibility: lockscreenMode ? "FullScreen" : "Windowed"
    width: Kirigami.Settings.isMobile ? 400 : 550
    height: Kirigami.Settings.isMobile ? 650 : 500
    title: "Phone"

    color: Kirigami.Theme.backgroundColor

    pageStack.initialPage: CallPage {}

    function selectModem() {
        const deviceUniList = DeviceUtils.deviceUniList;
        if (deviceUniList.length === 0) {
            console.warn("Modem devices not found");
            return "";
        }
        if (deviceUniList.length === 1)
            return deviceUniList[0];

        console.log("TODO: select device uni");
    }

    Connections {
        target: ActiveCallModel

        function onActiveChanged() {
            if (!ActiveCallModel.active) {
                console.log("There is no call ongoing, exiting");
                Qt.quit();
            }
        }
    }

    Component {
        id: callPageComponent
        CallPage {
        }
    }
}
