// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

// More information shown when a post is expanded, such as who favorited it etc.
ColumnLayout {
    id: root

    required property bool selected
    required property int visibility
    required property bool wasEdited
    required property string editedAt
    required property int favouritesCount
    required property int reblogsCount
    required property var application
    required property string absoluteTime
    required property string postId

    spacing: Kirigami.Units.largeSpacing

    Kirigami.Separator {
        visible: root.selected
        Layout.fillWidth: true
    }

    QQC2.Label {
        text: i18nc("Posted on 26 March 2024 Tuesday 20:43:18 +03", "Posted on %1", root.absoluteTime)
        elide: Text.ElideRight
        color: Kirigami.Theme.disabledTextColor
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
            icon.name: "favorite-favorited"
            icon.color: Kirigami.Theme.textColor
            iconMask: true
            visible: root.favouritesCount > 0
            text: {
                if (root.favouritesCount === 0) {
                    return i18n("No favorites");
                } else {
                    return i18np("%1 favorite", "%1 favorites", root.favouritesCount);
                }
            }
            closable: false
            checkable: false
            enabled: visible
            onClicked: {
                pageStack.push(socialGraphComponent, {
                    name: "favourited_by",
                    statusId: root.postId,
                    count: root.favouritesCount
                });
            }
        }

        Kirigami.Chip {
            icon.name: "boost-boosted"
            icon.color: Kirigami.Theme.textColor
            iconMask: true
            text: {
                if (root.reblogsCount === 0) {
                    return i18n("No boosts");
                } else {
                    return i18np("%1 boost", "%1 boosts", root.reblogsCount);
                }
            }
            closable: false
            checkable: false
            visible: root.reblogsCount > 0
            enabled: visible
            onClicked: {
                pageStack.push(socialGraphComponent, {
                    name: "reblogged_by",
                    statusId: root.postId,
                    count: root.reblogsCount
                });
            }
        }

        Kirigami.Chip {
            icon.name: "applications-other-symbolic"
            visible: root.application && root.application.name
            text: root.application && root.application.name ? root.application.name : ''
            closable: false
            checkable: false

            onClicked: Qt.openUrlExternally(root.application.website)
        }
    }
}
