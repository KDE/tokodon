// SPDX-FileCopyrightText: 2022 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.tokodon

import ".."

QQC2.Popup {
    id: tones

    signal chosen(string emoji)

    Component.onCompleted: {
        tonesList.currentIndex = 0;
        tonesList.forceActiveFocus();
    }

    required property string shortName
    required property string unicode
    required property int categoryIconSize
    width: tones.categoryIconSize * tonesList.count + 2 * padding
    height: tones.categoryIconSize + 2 * padding
    y: -height
    padding: 2
    modal: true
    dim: true
    clip: false
    onOpened: x = Math.min(parent.mapFromGlobal(QQC2.Overlay.overlay.width - tones.width, 0).x, -(width - parent.width) / 2)
    background: PopupShadow {}

    ListView {
        id: tonesList
        width: parent.width
        height: parent.height
        orientation: Qt.Horizontal
        model: EmojiModel.tones(tones.shortName)
        keyNavigationEnabled: true
        keyNavigationWraps: true

        delegate: EmojiDelegate {
            id: emojiDelegate
            checked: tonesList.currentIndex === model.index
            emoji: modelData.unicode
            name: modelData.shortName

            width: tones.categoryIconSize
            height: width

            Keys.onEnterPressed: clicked()
            Keys.onReturnPressed: clicked()
            onClicked: {
                tones.chosen(modelData.unicode)
                EmojiModel.emojiUsed(AccountManager.selectedAccount, name)
                tones.close()
            }
        }
    }
}
