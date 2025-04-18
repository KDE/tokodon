// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon


// This is the top part of the status that contains the author information, and some more actions
ClickableIdentityInfo {
    id: root

    required property string relativeTime
    required property bool wasEdited
    required property bool selected
    required property string absoluteTime
    required property string editedAt
    required property int visibility

    signal moreOpened(parentItem: var)

    onClicked: Navigation.openAccount(identity.id)

    accessibleName: i18nc("@info", "Post Author")
    accessibleDescription: root.identity.displayName

    Item {
        Layout.fillWidth: true
    }

    Kirigami.Heading {
        id: heading
        font.pixelSize: Config.defaultFont.pixelSize + 1
        font.pointSize: -1
        text: root.relativeTime
        color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
        elide: Text.ElideRight
        visible: !root.selected

        HoverHandler {
            id: hover
        }

        QQC2.ToolTip.text: root.absoluteTime
        QQC2.ToolTip.visible: hover.hovered && QQC2.ToolTip.text !== ""
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        Layout.alignment: Qt.AlignBaseline
    }

    InteractionButton {
        iconName: "document-edit"
        tooltip: i18nc("Edited on <datetime>", "Edited on %1", root.editedAt)
        visible: !root.selected && root.wasEdited

        Layout.alignment: Qt.AlignVCenter
    }

    Kirigami.Icon {
        visible: !root.selected
        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
        Layout.preferredHeight: Layout.preferredWidth
        source: {
            switch (root.visibility) {
                case Post.Public:
                    return "kstars_xplanet";
                case Post.Unlisted:
                    return "unlock";
                case Post.Private:
                    return "lock";
                case Post.Direct:
                    return "mail-message";
                default:
                    return "kstars_xplanet";
            }
        }

        HoverHandler {
            id: hover2
        }

        QQC2.ToolTip.text: {
            switch (root.visibility) {
                case Post.Public:
                    return i18n("Public");
                case Post.Unlisted:
                    return i18n("Unlisted");
                case Post.Private:
                    return i18n("Private");
                case Post.Direct:
                    return i18n("Direct Message");
                default:
                    return i18n("Public");
            }
        }
        QQC2.ToolTip.visible: hover2.hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        Layout.alignment: Qt.AlignVCenter
    }

    InteractionButton {
        id: moreButton

        iconName: 'overflow-menu'
        tooltip: i18nc("Show more options", "More actions")

        Layout.alignment: Qt.AlignVCenter
        Layout.preferredWidth: implicitHeight

        onClicked: root.moreOpened(moreButton)
    }
}
