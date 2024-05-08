// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
// SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.OverlayDrawer {
    id: drawer

    modal: false
    width: Kirigami.Units.gridUnit * 5
    height: applicationWindow().height
    edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    parent: QQC2.Overlay.overlay
    x: 0
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

        QQC2.ScrollView {
            id: scrollView

            Layout.fillWidth: true
            Layout.fillHeight: true
            QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            contentWidth: -1 // disable horizontal scroll

            ColumnLayout {
                id: column

                width: scrollView.width
                spacing: 0

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("History")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    text: i18n("History")
                    icon.name: "clock"
                    checked: pageStack.currentItem === page
                    enabled: !applicationWindow().lockscreenMode
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page)
                            applicationWindow().switchToPage(page, 0);
                        else
                            checked = Qt.binding(function() {
                                return applicationWindow().pageStack.currentItem === page;
                            });
                    }
                }

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Contacts")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    text: i18n("Contacts")
                    icon.name: "view-pim-contacts"
                    checked: pageStack.currentItem === page
                    enabled: !applicationWindow().lockscreenMode
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page)
                            applicationWindow().switchToPage(page, 0);
                        else
                            checked = Qt.binding(function() {
                                return applicationWindow().pageStack.currentItem === page;
                            });
                    }
                }

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Dialer")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    text: i18n("Dialer")
                    icon.name: "call-start"
                    checked: pageStack.currentItem === page
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page)
                            applicationWindow().switchToPage(page, 0);
                        else
                            checked = Qt.binding(function() {
                                return applicationWindow().pageStack.currentItem === page;
                            });
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                }

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Settings")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    text: i18n("Settings")
                    icon.name: "settings-configure"
                    checked: pageStack.currentItem === page
                    enabled: !applicationWindow().lockscreenMode
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page)
                            applicationWindow().switchToPage(page, 0);
                        else
                            checked = Qt.binding(function() {
                                return applicationWindow().pageStack.currentItem === page;
                            });
                    }
                }

                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin
                    visible: applicationWindow().lockscreenMode
                    text: i18n("Quit")
                    icon.name: "window-close-symbolic"
                    onClicked: {
                        Qt.quit();
                    }
                }

            }

        }

    }

}
