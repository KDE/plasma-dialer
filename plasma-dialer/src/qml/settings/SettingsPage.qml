/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *  SPDX-FileCopyrightText: 2022 Michael Lang <criticaltemp@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.phone.dialer 1.0
import org.kde.telephony 1.0

FormCard.FormCardPage {
    id: page

    title: i18n("Settings")

    // page animation
    property real yTranslate: 0

    function saveConfig() {
        Config.save()
        DialerUtils.syncSettings()
    }

    data: Connections {
        target: applicationWindow().pageStack
        function onCurrentIndexChanged() {
            page.saveConfig()
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit
        FormCard.FormButtonDelegate {
            id: about
            text: i18n("About")
            onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("About"))
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit
        FormCard.FormButtonDelegate {
            id: adaptiveCallBlocking
            text: i18n("Adaptive call blocking")
            onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("CallBlockSettings"))
        }

        FormCard.FormDelegateSeparator { above: adaptiveCallBlocking; below: callScreenAppearance }

        FormCard.FormComboBoxDelegate {
            id: callScreenAppearance
            displayMode: FormCard.FormComboBoxDelegate.Dialog
            text: i18n("Incoming call screen appearance")
            model: [i18n("Buttons"), i18n("Symmetric Swipe"), i18n("Asymmetric Swipe")]
            currentIndex: Config.answerControl
            onCurrentIndexChanged: Config.answerControl = currentIndex
        }
    }
}
