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
    title: i18n("Settings")

    // page animation
    property real yTranslate: 0

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

        transform: Translate { y: yTranslate }

        MobileForm.FormCard {
            Layout.fillWidth: true

            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormButtonDelegate {
                    id: about
                    text: i18n("About")
                    onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("About"))
                }
            }
        }
        
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                
                MobileForm.FormButtonDelegate {
                    id: adaptiveCallBlocking
                    text: i18n("Adaptive call blocking")
                    onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("CallBlockSettings"))
                }
                
                MobileForm.FormDelegateSeparator { above: adaptiveCallBlocking; below: callScreenAppearance }
                
                MobileForm.FormComboBoxDelegate {
                    id: callScreenAppearance
                    displayMode: MobileForm.FormComboBoxDelegate.Dialog
                    text: i18n("Incoming call screen appearance")
                    model: ["Buttons", "Symmetric Swipe", "Asymmetric Swipe"]
                    currentIndex: Config.answerControl
                    onCurrentIndexChanged: {
                        Config.answerControl = currentIndex
                    }
                }
            }
        }
    }
}
