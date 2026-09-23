// SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
// SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
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

    function checkActiveCall() {
        if (ActiveCallModel.initialized && !ActiveCallModel.active) {
            console.log("There is no call ongoing, exiting");
            Qt.quit();
        }
    }

    Component.onCompleted: {
        root.checkActiveCall();
    }

    Connections {
        target: ActiveCallModel

        function onInitializedChanged() {
            root.checkActiveCall();
        }

        function onActiveChanged() {
            root.checkActiveCall();
        }
    }

    Component {
        id: callPageComponent
        CallPage {
        }
    }
}
