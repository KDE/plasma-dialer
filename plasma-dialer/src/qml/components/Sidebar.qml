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
    height: applicationWindow().height

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
            implicitHeight: applicationWindow().pageStack.globalToolBar.preferredHeight

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
                property var page: applicationWindow().getPage("History")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                    }
                }
            }

            SidebarButton {
                text: i18n("Contacts")
                icon.name: "view-pim-contacts"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: applicationWindow().getPage("Contacts")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
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
                property var page: applicationWindow().getPage("Dialer")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                    }
                }
            }

            Item { Layout.fillHeight: true }
            Kirigami.Separator {
                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.smallSpacing
            }
            
            SidebarButton {
                text: i18n("Settings")
                icon.name: "settings-configure"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                checked: pageStack.currentItem === page
                property var page: applicationWindow().getPage("Settings")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                    }
                }
            }
        }
    }
}
