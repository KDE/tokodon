// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as KirigamiComponents
import org.kde.tokodon

Delegates.IndicatorItemDelegate {
    id: root

    required property string id
    required property string content
    required property var authorIdentity
    required property string relativeTime
    required property int conversationsCount
    required property string conversationId

    readonly property bool showSeparator: root.index !== conversationsCount - 1

    signal markAsRead(conversationId: string)

    onClicked: {
        Navigation.openPost(root.id)
        if (root.unread) {
            root.markAsRead(root.conversationId);
        }
    }

    contentItem: RowLayout {
        id: rowLayout

        spacing: Kirigami.Units.smallSpacing

        KirigamiComponents.Avatar {
            name: root.authorIdentity.displayName
            source: root.authorIdentity.avatarUrl
            Layout.rightMargin: Kirigami.Units.largeSpacing
            sourceSize.width: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
            sourceSize.height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
            Layout.preferredHeight: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0

            RowLayout {
                spacing: 0
                Layout.fillWidth: true
                QQC2.Label {
                    Layout.fillWidth: true
                    text: root.authorIdentity.displayNameHtml
                    elide: Text.ElideRight
                    font.weight: root.unread ? Font.Bold : Font.Normal
                }
                QQC2.Label {
                    text: root.relativeTime
                    color: Kirigami.Theme.disabledTextColor
                }
            }

            QQC2.Label {
                color: Kirigami.Theme.disabledTextColor
                Layout.fillWidth: true
                maximumLineCount: 1
                elide: Text.ElideRight
                text: root.content
            }
        }
    }
}
