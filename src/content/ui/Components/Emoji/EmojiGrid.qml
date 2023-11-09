// SPDX-FileCopyrightText: 2022 Tobias Fella
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

QQC2.ScrollView {
    id: emojiGrid

    property alias model: emojis.model
    property alias count: emojis.count
    required property int targetIconSize
    readonly property int emojisPerRow: emojis.width / targetIconSize
    required property bool withCustom
    readonly property var searchCategory: EmojiModel.Search
    required property QtObject header

    signal chosen(string unicode)

    onActiveFocusChanged: if (activeFocus) {
        emojis.forceActiveFocus()
    }

    GridView {
        id: emojis

        anchors.fill: parent
        anchors.rightMargin: parent.QQC2.ScrollBar.vertical.visible ? parent.QQC2.ScrollBar.vertical.width : 0

        currentIndex: -1
        keyNavigationEnabled: true
        onActiveFocusChanged: if (activeFocus && currentIndex === -1) {
            currentIndex = 0
        } else {
            currentIndex = -1
        }
        onModelChanged: currentIndex = -1
        reuseItems: true
        cacheBuffer: 1000

        cellWidth: emojis.width / emojiGrid.emojisPerRow
        cellHeight: emojiGrid.targetIconSize

        KeyNavigation.up: emojiGrid.header

        clip: true

        delegate: EmojiDelegate {
            id: emojiDelegate

            required property string unicode
            required property string identifier

            checked: emojis.currentIndex === model.index
            emoji: unicode
            name: identifier

            width: emojis.cellWidth
            height: emojis.cellHeight

            isImage: false //modelData.isCustom
            Keys.onEnterPressed: clicked()
            Keys.onReturnPressed: clicked()
            onClicked: {
                emojiGrid.chosen(unicode);
                EmojiModel.emojiUsed(AccountManager.selectedAccount, name);
            }
            Keys.onSpacePressed: pressAndHold()
            onPressAndHold: {
                if (EmojiModel.tones(modelData.shortName).length === 0) {
                    return;
                }
                let tones = tonesPopupComponent.createObject(emojiDelegate, {shortName: modelData.shortName, unicode: modelData.unicode, categoryIconSize: emojiGrid.targetIconSize})
                tones.open()
                tones.forceActiveFocus()
            }
            //showTones: !!modelData && EmojiModel.tones(modelData.shortName).length > 0
            showTones: false
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No emojis")
            visible: emojis.count === 0
        }
    }
    Component {
        id: tonesPopupComponent
        EmojiTonesPicker {
            onChosen: emojiGrid.chosen(emoji)
        }
    }
}
