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

import org.kde.phone.dialer 1.0
import org.kde.telephony 1.0

Kirigami.ScrollablePage {
    id: page
    title: i18n("Settings")
    topPadding: 0

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

    width: applicationWindow().width
    Kirigami.ColumnView.fillWidth: true

    Kirigami.FormLayout {
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: parent.forceActiveFocus()
        }

        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Adaptive blocking")
        }

        Controls.Switch {
            id: adaptiveBlocking
            checked: Config.adaptiveBlocking
            text: i18n("Ignore calls from unknown numbers")
            onToggled: Config.adaptiveBlocking = checked
        }

        ColumnLayout {
            visible: Config.adaptiveBlocking == true

            Column {
                leftPadding: Kirigami.Units.gridUnit
                bottomPadding: Kirigami.Units.gridUnit
                spacing: Kirigami.Units.largeSpacing

                Controls.RadioButton {
                    text: i18n("Immediately hang up")
                    checked: Config.ringOption == 0
                    onToggled: Config.ringOption = 0
                }
                Controls.RadioButton {
                    text: i18n("Ring without notification")
                    checked: Config.ringOption == 1
                    onToggled: Config.ringOption = 1
                }
                Controls.RadioButton {
                    text: i18n("Ring with silent notification")
                    checked: Config.ringOption == 2
                    onToggled: Config.ringOption = 2
                }
            }

            Kirigami.Heading {
                Kirigami.FormData.isSection: true
                text: i18n("Allowed exceptions")
                level: 3
            }

            Column {
                leftPadding: Kirigami.Units.gridUnit
                spacing: Kirigami.Units.largeSpacing

                Controls.Switch {
                    id: allowAnonymous
                    checked: Config.allowAnonymous
                    text: i18n("Anonymous numbers")
                    onToggled: Config.allowAnonymous = checked
                }

                Controls.Switch {
                    id: allowPreviousOutgoing
                    checked: Config.allowPreviousOutgoing
                    text: i18n("Existing outgoing call to number")
                    onToggled: Config.allowPreviousOutgoing = checked
                }

                Row {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.Switch {
                        id: allowCallback
                        checked: Config.allowCallback
                        text: i18n("Callback within")
                        onToggled: Config.allowCallback = checked
                    }

                    Controls.SpinBox {
                        id: callbackInterval
                        value: Config.callbackInterval
                        from: 1
                        to: 90
                        onValueModified: Config.callbackInterval = value
                        enabled: Config.allowCallback == true
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("minutes")
                        font: Kirigami.Theme.defaultFont
                        color: Kirigami.Theme.textColor
                    }
                }


                Kirigami.Heading {
                    topPadding: Kirigami.Units.largeSpacing
                    Kirigami.FormData.isSection: true
                    text: i18n("Numbers matching a pattern:")
                    level: 4
                }

                Column {
                    leftPadding: Kirigami.Units.gridUnit
                    spacing: Kirigami.Units.largeSpacing

                    Repeater {
                        model: Config.allowedPatterns

                        delegate: RowLayout {
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

                    RowLayout {
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

        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Other")
        }

        Controls.Button {
            text: i18n("About")
            icon.name: "help-about-symbolic"
            onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("About"))
        }
    }
}
