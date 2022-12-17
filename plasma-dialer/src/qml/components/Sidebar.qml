// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick 2.12
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.OverlayDrawer {
    id: drawer
    modal: false
    width: 90
    height: applicationWindow().height

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    contentItem: ColumnLayout {
        spacing: 0

        Kirigami.AbstractApplicationHeader {
            Layout.fillWidth: true
        }

        ColumnLayout {
            id: column
            spacing: 0

            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                text: i18n("History")
                icon.name: "clock"
                checked: pageStack.currentItem === page
                enabled: !applicationWindow().lockscreenMode
                property var page: applicationWindow().getPage("History")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                    }
                }
            }

            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                text: i18n("Contacts")
                icon.name: "view-pim-contacts"
                checked: pageStack.currentItem === page
                enabled: !applicationWindow().lockscreenMode
                property var page: applicationWindow().getPage("Contacts")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                    }
                }
            }

            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                text: i18n("Dialer")
                icon.name: "call-start"
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
                Layout.rightMargin: Kirigami.Units.smallSpacing
                Layout.leftMargin: Kirigami.Units.smallSpacing
            }
            
            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                text: i18n("Settings")
                icon.name: "settings-configure"
                checked: pageStack.currentItem === page
                enabled: !applicationWindow().lockscreenMode
                property var page: applicationWindow().getPage("Settings")
                onClicked: {
                    if (applicationWindow().pageStack.currentItem !== page) {
                        applicationWindow().switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                    }
                }
            }
            
            Kirigami.NavigationTabButton {
                Layout.fillWidth: true
                width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                visible: applicationWindow().lockscreenMode
                text: i18n("Quit")
                icon.name: "window-close-symbolic"
                onClicked: {
                    Qt.quit()
                }
            }
        }
    }
}
