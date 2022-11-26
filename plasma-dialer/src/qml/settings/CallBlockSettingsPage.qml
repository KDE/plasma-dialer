/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *  SPDX-FileCopyrightText: 2022 Michael Lang <criticaltemp@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

import org.kde.phone.dialer 1.0
import org.kde.telephony 1.0

Kirigami.ScrollablePage {
    id: page
    title: i18n("Adaptive Call Blocking")

    function saveConfig() {
        Config.save()
        DialerUtils.syncSettings()
    }

    Connections {
        target: applicationWindow().pageStack
        function onCurrentIndexChanged() {
            page.saveConfig()
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit

    ColumnLayout {
        spacing: 0
        width: page.width

        MobileForm.FormCard {
            Layout.fillWidth: true

            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormSwitchDelegate {
                    id: adaptiveBlocking
                    checked: Config.adaptiveBlocking
                    text: i18n("Ignore calls from unknown numbers")
                    onToggled: Config.adaptiveBlocking = checked
                }
                
            }
        }
        
        MobileForm.FormCard {
            visible: Config.adaptiveBlocking == true
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            
            contentItem: ColumnLayout {
                spacing: 0
                
                MobileForm.FormCardHeader {
                    title: i18n("When a call is an incoming from an unknown number")
                }
                
                MobileForm.FormRadioDelegate {
                    text: i18n("Immediately hang up")
                    checked: Config.ringOption == 0
                    onToggled: Config.ringOption = 0
                }
                MobileForm.FormRadioDelegate {
                    text: i18n("Ring without notification")
                    checked: Config.ringOption == 1
                    onToggled: Config.ringOption = 1
                }
                MobileForm.FormRadioDelegate {
                    text: i18n("Ring with silent notification")
                    checked: Config.ringOption == 2
                    onToggled: Config.ringOption = 2
                }
            }
        }
        
        MobileForm.FormCard {
            visible: Config.adaptiveBlocking == true
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            
            contentItem: ColumnLayout {
                spacing: 0
                
                MobileForm.FormCardHeader {
                    title: i18n("Allowed exceptions")
                }
                
                MobileForm.FormCheckDelegate {
                    id: allowAnonymous
                    checked: Config.allowAnonymous
                    text: i18n("Anonymous numbers")
                    onToggled: Config.allowAnonymous = checked
                }

                MobileForm.FormCheckDelegate {
                    id: allowPreviousOutgoing
                    checked: Config.allowPreviousOutgoing
                    text: i18n("Existing outgoing call to number")
                    onToggled: Config.allowPreviousOutgoing = checked
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.smallSpacing

                    MobileForm.FormCheckDelegate {
                        id: allowCallback
                        checked: Config.allowCallback
                        text: i18n("Callback within")
                        onToggled: Config.allowCallback = checked
                    }

                    Controls.SpinBox {
                        Layout.alignment: Qt.AlignVCenter
                        id: callbackInterval
                        value: Config.callbackInterval
                        from: 1
                        to: 90
                        onValueModified: Config.callbackInterval = value
                        enabled: Config.allowCallback == true
                    }

                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.rightMargin: Kirigami.Units.gridUnit
                        text: i18n("minutes")
                        font: Kirigami.Theme.defaultFont
                        color: Kirigami.Theme.textColor
                    }
                }
            }
        }
        
        MobileForm.FormCard {
            visible: Config.adaptiveBlocking == true
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            
            contentItem: ColumnLayout {
                spacing: 0
                
                MobileForm.FormCardHeader {
                    title: i18n("Allowed phone number exceptions")
                }
                
                Repeater {
                    model: Config.allowedPatterns

                    delegate: MobileForm.AbstractFormDelegate {
                        background: Item {}
                        
                        contentItem: RowLayout {
                            Controls.TextField {
                                id: phoneField
                                placeholderText: i18n("Add new pattern")
                                text: modelData
                                inputMethodHints: Qt.ImhDialableCharactersOnly
                                Layout.fillWidth: true
                                onEditingFinished: Config.allowedPatterns[index] = text
                            }
                            Controls.Button {
                                icon.name: "list-remove"
                                implicitWidth: implicitHeight
                                onClicked: {
                                    let newList = Config.allowedPatterns.filter((value, index) => index != model.index)
                                    Config.allowedPatterns = newList
                                }
                            }
                        }
                    }
                }
                
                MobileForm.AbstractFormDelegate {
                    background: Item {}
                    
                    contentItem: RowLayout {
                        Controls.TextField {
                            id: toAddPattern
                            Layout.fillWidth: true
                            placeholderText: i18n("Add new pattern")
                            inputMethodHints: Qt.ImhDialableCharactersOnly
                        }

                        Controls.Button {
                            icon.name: "list-add"
                            implicitWidth: implicitHeight
                            enabled: toAddPattern.text.length > 0
                            onClicked: {
                                let numbers = Config.allowedPatterns
                                numbers.push(toAddPattern.text)
                                Config.allowedPatterns = numbers
                                toAddPattern.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
}

