// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.OverlayDrawer {
    id: drawer
    modal: false
    width: 200
    height: appWindow.height

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    contentItem: ColumnLayout {
        spacing: 0

        QQC2.ToolBar {
            Layout.fillWidth: true
            implicitHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.smallSpacing

            Item {
                anchors.fill: parent
                Kirigami.Heading {
                    level: 1
                    text: i18n("Phone")
                    anchors.left: parent.left
                    anchors.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        ColumnLayout {
            id: column
            spacing: 0
            Layout.margins: Kirigami.Units.smallSpacing

            SidebarButton {
                text: i18n("History")
                icon.name: "clock"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appWindow.getPage("History")
                onClicked: {
                    if (appWindow.pageStack.currentItem !== page) {
                        appWindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appWindow.pageStack.currentItem === page; });
                    }
                }
            }

            SidebarButton {
                text: i18n("Contacts")
                enabled: false // TODO
                icon.name: "view-pim-contacts"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appWindow.getPage("Contacts")
                onClicked: {
                    if (appWindow.pageStack.currentItem !== page) {
                        appWindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appWindow.pageStack.currentItem === page; });
                    }
                }
            }

            SidebarButton {
                text: i18n("Dialer")
                icon.name: "call-start"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appWindow.getPage("Dialer")
                onClicked: {
                    if (appWindow.pageStack.currentItem !== page) {
                        appWindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appWindow.pageStack.currentItem === page; });
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }
}
