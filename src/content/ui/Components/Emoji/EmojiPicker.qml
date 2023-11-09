// SPDX-FileCopyrightText: 2022 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

ColumnLayout {
    id: root

    readonly property var currentEmojiModel: EmojiModel.categories
    readonly property int categoryIconSize: Math.round(Kirigami.Units.gridUnit * 2.5)
    readonly property var currentCategory: currentEmojiModel[categories.currentIndex].category
    readonly property alias categoryCount: categories.count

    signal chosen(string emoji)

    function clearSearchField() {
        searchField.text = ""
    }

    onActiveFocusChanged: if (activeFocus) {
        searchField.forceActiveFocus();
    }

    spacing: 0

    QQC2.ScrollView {
        Layout.fillWidth: true
        Layout.preferredHeight: root.categoryIconSize + QQC2.ScrollBar.horizontal.height
        QQC2.ScrollBar.horizontal.height: QQC2.ScrollBar.horizontal.visible ? QQC2.ScrollBar.horizontal.implicitHeight : 0

        ListView {
            id: categories
            clip: true
            focus: true
            orientation: ListView.Horizontal

            Keys.onReturnPressed: if (emojiGrid.count > 0) emojiGrid.focus = true
            Keys.onEnterPressed: if (emojiGrid.count > 0) emojiGrid.focus = true

            KeyNavigation.down: emojiGrid.count > 0 ? emojiGrid : categories
            KeyNavigation.tab: emojiGrid.count > 0 ? emojiGrid : categories

            keyNavigationEnabled: true
            keyNavigationWraps: true
            Keys.forwardTo: searchField
            interactive: width !== contentWidth

            model: root.currentEmojiModel
            Component.onCompleted: categories.forceActiveFocus()

            delegate: emojiDelegate
        }
    }

    Kirigami.Separator {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
    }

    Kirigami.SearchField {
        id: searchField
        Layout.margins: Kirigami.Units.smallSpacing
        Layout.fillWidth: true

        /**
         * The focus is manged by the parent and we don't want to use the standard
         * shortcut as it could block other SearchFields from using it.
         */
        focusSequence: ""

        onAccepted: EmojiModel.searchString = text
    }

    EmojiGrid {
        id: emojiGrid
        targetIconSize: root.categoryIconSize  // Custom emojis are bigger
        model: EmojiModel.model
        Layout.fillWidth: true
        Layout.fillHeight: true
        withCustom: true
        onChosen: root.chosen(unicode)
        header: categories
        Keys.forwardTo: searchField
    }

    Component {
        id: emojiDelegate
        Kirigami.NavigationTabButton {
            width: root.categoryIconSize
            height: width
            checked: categories.currentIndex === model.index
            text: modelData.name
            QQC2.ToolTip.text: modelData.i18nName
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.visible: hovered
            onClicked: {
                EmojiModel.category = modelData.category;
                categories.currentIndex = index;
                categories.focus = true;
            }
        }
    }
}
