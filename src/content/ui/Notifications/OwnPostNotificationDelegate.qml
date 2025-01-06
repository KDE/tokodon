// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import org.kde.tokodon

/**
 * Our own post, for example when it's favorited or boosted.
 */
NotificationDelegate {
    id: root

    required property string id
    required property string content
    required property var authorIdentity
    required property string relativeTime
    required property var timelineModel
    required property bool isGroup
    required property bool isInGroup
    required property int numInGroup
    required property var notificationActorIdentity
    required property int type

    ColumnLayout {
        id: rowLayout

        spacing: Kirigami.Units.smallSpacing

        // interaction label
        RowLayout {
            spacing: 0

            Loader {
                active: root.isGroup

                sourceComponent: GroupInteractionLabel {
                    type: root.type
                    notificationActorIdentity: root.notificationActorIdentity
                    numInGroup: root.numInGroup
                }
            }

            Loader {
                active: !root.isGroup

                sourceComponent: UserInteractionLabel {
                    type: root.type
                    notificationActorIdentity: root.notificationActorIdentity
                }
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Label {
                text: root.relativeTime
                color: Kirigami.Theme.disabledTextColor
            }
        }

        PostContent {
            content: root.content
            expandedPost: false
            secondary: true
            shouldOpenInternalLinks: true
        }
    }
}
