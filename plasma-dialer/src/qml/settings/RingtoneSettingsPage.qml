/*
 *  SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtMultimedia

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.sounds 0.1 as Sounds
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasma.dialer
import org.kde.telephony

Kirigami.ScrollablePage {
    id: root

    title: i18n("Ringtone")

    // Track current selection (empty = default)
    property string selectedRingtone: Config.customRingtone

    function saveConfig() {
        Config.customRingtone = selectedRingtone;
        Config.save();
        DialerUtils.syncSettings();
    }

    function replacePrefix(path) {
        return path.replace('file://', '');
    }

    function playablePath(str) {
        return str.includes('file://') ? str : ('file://' + str);
    }

    onVisibleChanged: audioPlayer.stop()

    MediaPlayer {
        id: audioPlayer
        audioOutput: AudioOutput {}
    }

    function playSound(path) {
        audioPlayer.stop();
        if (path && path.length > 0) {
            audioPlayer.source = playablePath(path);
            audioPlayer.play();
        }
    }

    Component.onDestruction: {
        audioPlayer.stop();
    }

    ListView {
        id: listView
        model: Sounds.SoundsModel {
            id: soundsModel
            notification: false
            theme: RingtoneUtils.getSoundTheme()
        }
        currentIndex: -1

        header: ColumnLayout {
            width: listView.width
            spacing: 0

            // Select from files option
            Delegates.RoundedItemDelegate {
                id: selectFromFilesItem
                text: i18n("Select from files...")
                Layout.fillWidth: true
                onClicked: fileDialog.open()
                contentItem: RowLayout {
                    Delegates.DefaultContentItem {
                        itemDelegate: selectFromFilesItem
                    }
                    RadioButton {
                        checked: {
                            if (root.selectedRingtone === "") {
                                return false;
                            }
                            return !RingtoneUtils.soundInSoundTheme(replacePrefix(root.selectedRingtone));
                        }
                    }
                }
            }

            // Default option
            Delegates.RoundedItemDelegate {
                id: defaultItem
                text: i18n("Default (System)")
                Layout.fillWidth: true

                onClicked: {
                    root.selectedRingtone = "";
                    let defaultPath = RingtoneUtils.getDefaultRingtoneLocation();
                    if (defaultPath) {
                        playSound(defaultPath);
                    }
                }

                contentItem: RowLayout {
                    Delegates.DefaultContentItem {
                        itemDelegate: defaultItem
                    }
                    RadioButton {
                        checked: root.selectedRingtone === ""
                    }
                }
            }
        }

        // Theme sounds
        delegate: Delegates.RoundedItemDelegate {
            id: soundDelegate

            required property string ringtoneName
            required property string sourceUrl
            required property int index

            text: ringtoneName

            onClicked: {
                root.selectedRingtone = replacePrefix(sourceUrl);
                playSound(sourceUrl);
            }

            contentItem: RowLayout {
                Delegates.DefaultContentItem {
                    itemDelegate: soundDelegate
                }
                RadioButton {
                    checked: replacePrefix(root.selectedRingtone) === replacePrefix(soundDelegate.sourceUrl)
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: i18n("Choose a ringtone")
        currentFolder: StandardPaths.standardLocations(StandardPaths.MusicLocation)[0]
        nameFilters: [
            "Audio files (*.wav *.mp3 *.ogg *.oga *.aac *.flac *.webm *.mka *.opus)",
            "All files (*)"
        ]
        onAccepted: {
            root.selectedRingtone = replacePrefix(selectedFile.toString());
            console.log(root.selectedRingtone)
            playSound(selectedFile.toString());
        }
    }

    // Save on page exit
    data: Connections {
        function onCurrentIndexChanged() {
            root.saveConfig();
        }
        target: applicationWindow().pageStack
    }
}
