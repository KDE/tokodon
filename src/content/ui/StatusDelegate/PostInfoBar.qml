// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private
import Qt5Compat.GraphicalEffects
import Qt.labs.qmlmodels 1.0

// This is the top part of the status that contains the author information, and some more actions
InlineIdentityInfo {
    id: infoBar

    identity: root.authorIdentity
    secondary: root.secondary
    visible: !filtered

    signal moreOpened

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
        QQC2.ToolTip.visible: hover.hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        Layout.alignment: Qt.AlignBaseline
    }

    InteractionButton {
        iconName: "document-edit"
        tooltip: "Edited on " + root.editedAt
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
        iconName: 'overflow-menu'
        tooltip: i18nc("Show more options", "More")

        Layout.alignment: Qt.AlignVCenter
        Layout.preferredWidth: implicitHeight

        onClicked: infoBar.moreOpened()
    }
}