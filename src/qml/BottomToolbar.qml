/*
 * SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
import QtGraphicalEffects 1.12

ToolBar {
    id: toolbarRoot
    property double iconSize: Math.round(Kirigami.Units.gridUnit * 1.5)
    property double shrinkIconSize: Math.round(Kirigami.Units.gridUnit * 1.1)
    property double fontSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 0.8)
    property double shrinkFontSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 0.7)
    
    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
        
        layer.enabled: true
        layer.effect: DropShadow {
            color: Qt.rgba(0.0, 0.0, 0.0, 0.3)
            radius: 6
            samples: 8
        }
    }
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        Repeater {
            model: [
                {
                    icon: "clock",
                    page: historyPage
                },
                {
                    icon: "view-pim-contacts",
                    page: contactsPage
                },
                {
                    icon: "call-start",
                    page: dialerPage
                }
            ]
            
            Rectangle {
                Layout.minimumWidth: parent.width / 3
                Layout.maximumWidth: parent.width / 3
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                Layout.alignment: Qt.AlignCenter
                
                Kirigami.Theme.colorSet: Kirigami.Theme.Window
                Kirigami.Theme.inherit: false

                color: mouseArea.pressed ? Qt.darker(Kirigami.Theme.backgroundColor, 1.1) : 
                       mouseArea.containsMouse ? Qt.darker(Kirigami.Theme.backgroundColor, 1.03) : Kirigami.Theme.backgroundColor
                       
                Behavior on color {
                    ColorAnimation { 
                        duration: 100 
                        easing.type: Easing.InOutQuad
                    }
                }
                
                MouseArea {
                    id: mouseArea
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        if (!modelData.page.visible) {
                            root.switchToPage(modelData.page, 0)
                        }
                    }
                    onPressed: {
                        widthAnim.to = toolbarRoot.shrinkIconSize;
                        heightAnim.to = toolbarRoot.shrinkIconSize;
                        fontAnim.to = toolbarRoot.shrinkFontSize;
                        widthAnim.restart();
                        heightAnim.restart();
                        fontAnim.restart();
                    }
                    onReleased: {
                        if (!widthAnim.running) {
                            widthAnim.to = toolbarRoot.iconSize;
                            widthAnim.restart();
                        }
                        if (!heightAnim.running) {
                            heightAnim.to = toolbarRoot.iconSize;
                            heightAnim.restart();
                        }
                        if (!fontAnim.running) {
                            fontAnim.to = toolbarRoot.fontSize;
                            fontAnim.restart();
                        }
                    }
                }
                
                ColumnLayout {
                    id: itemColumn
                    anchors.fill: parent
                    spacing: Kirigami.Units.smallSpacing
                    
                    Kirigami.Icon {
                        color: modelData.page.visible ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                        source: modelData.icon
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        Layout.preferredHeight: toolbarRoot.iconSize
                        Layout.preferredWidth: toolbarRoot.iconSize
                        
                        ColorAnimation on color {
                            easing.type: Easing.Linear
                        }
                        NumberAnimation on Layout.preferredWidth {
                            id: widthAnim
                            easing.type: Easing.Linear
                            duration: 130
                            onFinished: {
                                if (widthAnim.to !== toolbarRoot.iconSize && !mouseArea.pressed) {
                                    widthAnim.to = toolbarRoot.iconSize;
                                    widthAnim.start();
                                }
                            }
                        }
                        NumberAnimation on Layout.preferredHeight {
                            id: heightAnim
                            easing.type: Easing.Linear
                            duration: 130
                            onFinished: {
                                if (heightAnim.to !== toolbarRoot.iconSize && !mouseArea.pressed) {
                                    heightAnim.to = toolbarRoot.iconSize;
                                    heightAnim.start();
                                }
                            }
                        }
                    }
                    
                    Label {
                        color: modelData.page.visible ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                        text: i18n(modelData.page.title)
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideMiddle
                        font.pointSize: toolbarRoot.fontSize
                        
                        ColorAnimation on color {
                            easing.type: Easing.Linear
                        }
                        NumberAnimation on font.pointSize {
                            id: fontAnim
                            easing.type: Easing.Linear
                            duration: 130
                            onFinished: {
                                if (fontAnim.to !== toolbarRoot.fontSize && !mouseArea.pressed) {
                                    fontAnim.to = toolbarRoot.fontSize;
                                    fontAnim.start();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
} 
