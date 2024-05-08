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
import "call"

Kirigami.ApplicationWindow {
    id: root

    readonly property bool smallMode: applicationWindow().height < Kirigami.Units.gridUnit * 28
    property bool lockscreenMode: ScreenSaverUtils.getActive()
    property bool isWidescreen: root.width >= root.height

    signal ussdUserInitiated()

    function updateLockscreenMode(mode) {
        root.lockscreenMode = mode;
    }

    function getPage(name) {
        switch (name) {
        case "History":
            return pagePool.loadPage("qrc:/HistoryPage.qml");
        case "Contacts":
            return pagePool.loadPage("qrc:/ContactsPage.qml");
        case "Dialer":
            return pagePool.loadPage("qrc:/DialerPage.qml");
        case "Call":
            return pagePool.loadPage("qrc:/call/CallPage.qml");
        case "Settings":
            return pagePool.loadPage("qrc:/settings/SettingsPage.qml");
        case "CallBlockSettings":
            return pagePool.loadPage("qrc:/settings/CallBlockSettingsPage.qml");
        case "About":
            return pagePool.loadPage("qrc:/AboutPage.qml");
        }
    }

    function switchToPage(page, depth) {
        if (!page)
            return ;

        while (pageStack.depth > depth)pageStack.pop()
        // page switch animation
        yAnim.target = page;
        yAnim.properties = "yTranslate";
        anim.target = page;
        anim.properties = "contentItem.opacity";
        if (page.header)
            anim.properties += ",header.opacity";

        yAnim.restart();
        anim.restart();
        pageStack.push(page);
        page.forceActiveFocus();
    }

    // switch between bottom toolbar and sidebar
    function changeNav(toWidescreen) {
        if (toWidescreen) {
            if (footer != null) {
                footer.destroy();
                footer = null;
            }
            sidebarLoader.active = true;
            globalDrawer = sidebarLoader.item;
        } else {
            sidebarLoader.active = false;
            globalDrawer = null;
            let bottomToolbar = Qt.createComponent("qrc:/components/BottomToolbar.qml");
            footer = bottomToolbar.createObject(root);
        }
    }

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

    function call(number) {
        getPage("Dialer").pad.number = number;
        switchToPage(getPage("Dialer"), 0);
    }

    wideScreen: false
    pageStack.globalToolBar.canContainHandles: true
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton
    pageStack.popHiddenPages: true
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn
    // needs to work with 360x720 (+ panel heights)
    minimumWidth: 300
    minimumHeight: minimumWidth + 1
    width: Kirigami.Settings.isMobile ? 400 : 550
    height: Kirigami.Settings.isMobile ? 650 : 500
    visibility: lockscreenMode ? "FullScreen" : "Windowed"
    title: i18n("Phone")
    onIsWidescreenChanged: changeNav(isWidescreen)
    Component.onCompleted: {
        // initial page and nav type
        switchToPage(getPage("Dialer"), 1);
        changeNav(isWidescreen);
    }
    onUssdUserInitiated: {
        ussdSheet.open(); // open it already since async interaction is not immediate
    }

    Kirigami.PagePool {
        id: pagePool
    }

    // page switch animation
    NumberAnimation {
        id: anim

        from: 0
        to: 1
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: yAnim

        from: Kirigami.Units.gridUnit * 3
        to: 0
        duration: Kirigami.Units.longDuration * 3
        easing.type: Easing.OutQuint
    }

    Loader {
        id: sidebarLoader

        source: "qrc:/components/Sidebar.qml"
        active: false
    }

    USSDSheet {
        id: ussdSheet

        onResponseReady: {
            const deviceUni = applicationWindow().selectModem();
            UssdUtils.respond(deviceUni, response);
        }
        onCancelSessionRequested: {
            const deviceUni = applicationWindow().selectModem();
            UssdUtils.cancel(deviceUni);
        }
    }

    ImeiSheet {
        id: imeiSheet

        function show() {
            imeiSheet.imeis = DeviceUtils.equipmentIdentifiers;
            imeiSheet.open();
        }

    }

    Connections {
        // TODO: also activate on Dialing state

        function onCallStateChanged(callData) {
            if (callData.state === DialerTypes.CallState.Active)
                getPage("Dialer").pad.clear();

        }

        target: CallUtils
    }

    Connections {
        function onNotificationReceived(deviceUni, message) {
            ussdSheet.showNotification(message);
        }

        function onStateChanged(deviceUni, state) {
            ussdSheet.changeState(state);
        }

        function onErrorReceived(deviceUni, error) {
            ussdSheet.showError(error);
        }

        function onInitiated(deviceUni, command) {
            ussdSheet.open();
        }

        target: UssdUtils
    }

    contextDrawer: Kirigami.ContextDrawer {
    }

}
