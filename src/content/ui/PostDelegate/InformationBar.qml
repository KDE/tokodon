// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private
import Qt.labs.qmlmodels 1.0

// More information shown when a post is expanded, such as who favorited it etc.
ColumnLayout {
    spacing: Kirigami.Units.largeSpacing

    Kirigami.Separator {
        visible: root.selected
        Layout.fillWidth: true
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing

        QQC2.Label {
            text: "Posted on " + root.absoluteTime
            elide: Text.ElideRight
            color: Kirigami.Theme.disabledTextColor
        }

        QQC2.Label {
            visible: root.application && root.application.name
            text: root.application && root.application.name ? i18n("via %1", root.application.name) : ''
            elide: Text.ElideRight
            Layout.fillWidth: true
            color: Kirigami.Theme.disabledTextColor

            HoverHandler {
                cursorShape: hasWebsite ? Qt.PointingHandCursor : Qt.ArrowCursor
                onHoveredChanged: if (hovered) {
                    applicationWindow().hoverLinkIndicator.text = root.application.website;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: hasWebsite ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: Qt.openUrlExternally(root.application.website)
            }
        }
    }

    Flow {
        spacing: Kirigami.Units.largeSpacing

        Layout.fillWidth: true

        Kirigami.Chip {
            closable: false
            checkable: false
            enabled: false
            icon.name:
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
            text:
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

        Kirigami.Chip {
            icon.name: "document-edit"
            text: "Edited on " + root.editedAt
            closable: false
            checkable: false
            visible: root.wasEdited
            enabled: false
        }

        Kirigami.Chip {
            icon.name: "tokodon-post-favorited"
            icon.color: Kirigami.Theme.textColor
            iconMask: true
            text: i18np("%1 Favorite", "%1 Favorites", root.favouritesCount)
            closable: false
            checkable: false
            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }
            onClicked: {
                pageStack.push(socialGraphComponent, {
                    name: "favourited_by",
                    statusId: root.id,
                    count: root.favouritesCount
                });
            }
        }

        Kirigami.Chip {
            icon.name: "tokodon-post-boosted"
            icon.color: Kirigami.Theme.textColor
            iconMask: true
            text: i18np("%1 Boost", "%1 Boosts", root.reblogsCount)
            closable: false
            checkable: false
            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }
            onClicked: {
                pageStack.push(socialGraphComponent, {
                    name: "reblogged_by",
                    statusId: root.id,
                    count: root.reblogsCount
                });
            }
        }
    }
}