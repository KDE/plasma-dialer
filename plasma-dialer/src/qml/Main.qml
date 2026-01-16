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
import org.kde.plasma.dialer

import "call"

Kirigami.ApplicationWindow {
    id: root

    readonly property bool smallMode: applicationWindow().height < Kirigami.Units.gridUnit * 28
    property bool lockscreenMode: LockScreenUtils.lockscreenActive
    property bool isWidescreen: !LockScreenUtils.lockscreenActive && root.width >= root.height

    signal ussdUserInitiated()

    function getPage(name) {
        switch (name) {
        case "History":
            return pagePool.loadPage("HistoryPage.qml");
        case "Contacts":
            return pagePool.loadPage("ContactsPage.qml");
        case "Dialer":
            return pagePool.loadPage("DialerPage.qml");
        case "Call":
            return pagePool.loadPage("call/CallPage.qml");
        case "Settings":
            return pagePool.loadPage("settings/SettingsPage.qml");
        case "CallBlockSettings":
            return pagePool.loadPage("settings/CallBlockSettingsPage.qml");
        case "About":
            return pagePool.loadPage("AboutPage.qml");
        }
    }

    function switchToPage(page) {
        if (!page)
            return;

        while (pageStack.layers.depth > 1) pageStack.layers.pop();
        pageStack.clear();

        // page switch animation
        if (!lockscreenMode && page.hasOwnProperty("yTranslate")) {
            yAnim.target = page;
            yAnim.properties = "yTranslate";
            anim.target = page;
            anim.properties = "contentItem.opacity";
            if (page.header)
                anim.properties += ",header.opacity";
            yAnim.restart();
            anim.restart();
        }

        pageStack.push(page);
        page.forceActiveFocus();
    }

    // switch between bottom toolbar and sidebar
    function changeNav(toWidescreen) {
        if (footer != null) {
            footer.destroy();
            footer = null;
        }
        sidebarLoader.active = false;
        globalDrawer = null;

        // No navigation on lockscreen
        if (applicationWindow().lockscreenMode) {
            return;
        }

        if (toWidescreen) {
            sidebarLoader.active = true;
            globalDrawer = sidebarLoader.item;
        } else {
            let bottomToolbar = Qt.createComponent("components/BottomToolbar.qml");
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
        switchToPage(getPage("Dialer"));
    }

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
        switchToPage(getPage("Dialer"));
        changeNav(isWidescreen);

        applyCallScreenState();
    }

    onLockscreenModeChanged: {
        applyCallScreenState();

        // Apply navigation style
        changeNav(isWidescreen);
    }

    onUssdUserInitiated: {
        ussdSheet.open(); // open it already since async interaction is not immediate
    }

    // Performs the logic needed to open the call screen if necessary
    function applyCallScreenState() {
        const callPage = getPage("Call");

        if (ActiveCallModel.active && pageStack.layers.currentItem !== callPage) {
            pageStack.layers.push(callPage);
        }
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

        source: "components/Sidebar.qml"
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
        function onLockscreenLocked() {
            if (!ActiveCallModel.active) {
                console.log("There is no call ongoing and we are in the lockscreen, exiting");
                Qt.quit();
            }
        }

        target: LockScreenUtils
    }

    Connections {
        // Open call page when a call initiates
        function onActiveChanged() {
            root.applyCallScreenState();

            if (!ActiveCallModel.active) {
                if (root.lockscreenMode) {
                    // When the call exits, just quit on the menu
                    // We can't do this in applyCallScreenState(), because the initial call state is always active = false
                    console.log("There is no call ongoing, exiting");
                    Qt.quit();
                } else {
                    // Remove call page
                    if (pageStack.layers.currentItem === getPage("Call")) {
                        pageStack.layers.pop();
                    }
                }
            }
        }

        target: ActiveCallModel
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
}
