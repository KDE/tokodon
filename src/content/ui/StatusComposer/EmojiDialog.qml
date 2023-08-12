// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami

import "../Components/Emoji"
import "../Components"

QQC2.Popup {
    id: emojiPopup

    property bool closeOnChosen: true

    signal chosen(string emoji)

    onVisibleChanged: {
        if (!visible) {
            return
        }
        emojiPicker.forceActiveFocus()
    }

    background: PopupShadow {
        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }

    modal: true
    focus: true
    clip: false
    closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutsideParent
    margins: 0
    padding: 2

    implicitHeight: Kirigami.Units.gridUnit * 20 + 2 * padding
    width: Math.min(contentItem.categoryIconSize * 11 + 2 * padding, applicationWindow().width)
    contentItem: EmojiPicker {
        id: emojiPicker
        height: 400
        onChosen: {
            emojiPopup.chosen(emoji)
            if (emojiPopup.closeOnChosen) {
                emojiPicker.clearSearchField()
                emojiPopup.close()
            }
        }
    }
}
