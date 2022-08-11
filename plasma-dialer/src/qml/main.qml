// SPDX-FileCopyrightText: 2014 Aaron Seigo <aseigo@kde.org>
// SPDX-FileCopyrightText: 2014 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.LocalStorage 2.0

import org.kde.kirigami 2.19 as Kirigami

import org.kde.telephony 1.0

import "call"

Kirigami.ApplicationWindow {
    FontLoader { id: webFont; source: "qrc:/font-manrope" }

    wideScreen: false
    id: root

    pageStack.globalToolBar.preferredHeight : 0

    contextDrawer: Kirigami.ContextDrawer {}

    readonly property bool smallMode: applicationWindow().height < Kirigami.Units.gridUnit * 28

    // pop pages when not in use
    Connections {
        target: applicationWindow().pageStack
        function onCurrentIndexChanged() {
            // wait for animation to finish before popping pages
            timer.restart();
        }
    }
    
    Timer {
        id: timer
        interval: 300
        onTriggered: {
            let currentIndex = applicationWindow().pageStack.currentIndex;
            while (applicationWindow().pageStack.depth > (currentIndex + 1) && currentIndex >= 0) {
                applicationWindow().pageStack.pop();
            }
        }
    }

    Kirigami.PagePool { id: pagePool }

    function getContactIcon(name) {
        console.log("getContactIcon", name);
        switch (name.substr(0, 1).toUpperCase()) {
            case "А": return ":Contact-id-1";
            case "Б": return ":Contact-id-2";
            case "В": return ":Contact-id-3";
            case "Г": return ":Contact-id-4";
            case "Д": return ":Contact-id-5";
            case "Е": return ":Contact-id-6";
            case "Ё": return ":Contact-id-7";
            case "Ж": return ":Contact-id-8";
            case "З": return ":Contact-id-9";
            case "И": return ":Contact-id-10";
            case "Й": return ":Contact-id-11";
            case "К": return ":Contact-id-12";
            case "Л": return ":Contact-id-13";
            case "М": return ":Contact-id-14";
            case "Н": return ":Contact-id-15";
            case "О": return ":Contact-id-16";
            case "П": return ":Contact-id-17";
            case "Р": return ":Contact-id-18";
            case "С": return ":Contact-id-19";
            case "Т": return ":Contact-id-20";
            case "У": return ":Contact-id-21";
            case "Ф": return ":Contact-id-22";
            case "Х": return ":Contact-id-23";
            case "Ц": return ":Contact-id-24";
            case "Ч": return ":Contact-id-25";
            case "Ш": return ":Contact-id-26";
            case "Щ": return ":Contact-id-27";
            case "Ъ": return ":Contact-id-28";
            case "Ь": return ":Contact-id-29";
            case "Ы": return ":Contact-id-30";
            case "Э": return ":Contact-id-31";
            case "Ю": return ":Contact-id-32";
            case "Я": return ":Contact-id-33";
            default: return ":Contact-id-noname";
        }
    }

    function getPage(name) {
        switch (name) {
        case "History": return pagePool.loadPage("qrc:/HistoryPage.qml");
        case "Contacts": return pagePool.loadPage("qrc:/ContactsPage.qml");
        case "Dialer": return pagePool.loadPage("qrc:/DialerPage.qml");
        case "Call": return pagePool.loadPage("qrc:/call/CallPage.qml");
        case "Settings": return pagePool.loadPage("qrc:/SettingsPage.qml");
        case "About": return pagePool.loadPage("qrc:/AboutPage.qml");
        case "Incoming": return pagePool.loadPage("qrc:/call/IncomingPage.qml");
        }
    }

    property bool isWidescreen: root.width >= root.height

    function switchToPage(page, depth) {
        while (pageStack.depth > depth) pageStack.pop()
        pageStack.push(page)
        page.forceActiveFocus()
    }

    function selectModem() {
        const deviceUniList = DeviceUtils.deviceUniList()
        if (deviceUniList.length === 0) {
            console.warn("Modem devices not found")
            return ""
        }

        if (deviceUniList.length === 1) {
            return deviceUniList[0]
        }
        console.log("TODO: select device uni")
    }

    function call(number) {
        getPage("Dialer").pad.number = number
        switchToPage(getPage("Dialer"), 0)
    }

    Component.onCompleted: {
        // initial page and nav type
        switchToPage(getPage("Dialer"), 1);
    }

    Loader {
        id: sidebarLoader
        source: "qrc:/components/Sidebar.qml"
        active: false
    }

    USSDSheet {
        id: ussdSheet
        onResponseReady: {
            // TODO: debug
            // USSDUtils.respond(response)
        }
    }

    ImeiSheet {
        id: imeiSheet
        function show() {
            imeiSheet.imeis = DeviceUtils.equipmentIdentifiers()
            imeiSheet.open()
        }
    }

    Connections {
        target: CallUtils

        function onMissedCallsActionTriggered() {
            root.visible = true;
        }

        function onCallStateChanged(state) {
            if (CallUtils.callState === DialerTypes.CallState.Active) {
                getPage("Dialer").pad.number = ""
            }
            // TODO: also activate on Dialing state
        }
    }

    Connections {
        target: UssdUtils

        function onNotificationReceived(deviceUni, message) {
            ussdSheet.showNotification(message)
        }

        function onRequestReceived(deviceUni, message) {
            ussdSheet.showNotification(message, true)
        }

        function onInitiated(deviceUni, command) {
            ussdSheet.open()
        }
    }
}
