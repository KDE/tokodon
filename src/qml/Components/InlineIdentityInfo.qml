// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.tokodon


// Used everywhere, is a component with an Avatar, some labels and such smashed together.
// If you want to click on this (e.g. the user info used in a post) use ClickableIdentityInfo. Otherwise use this.
RowLayout {
    id: root

    required property var identity
    property bool admin: false
    property string ip
    readonly property alias avatar: avatar

    spacing: Kirigami.Units.largeSpacing

    KirigamiComponents.Avatar {
        id: avatar

        Layout.alignment: admin ? Qt.AlignCenter : Qt.AlignTop
        Layout.preferredHeight: Kirigami.Units.iconSizes.medium
        Layout.preferredWidth: Kirigami.Units.iconSizes.medium

        source: root.identity?.avatarUrl ?? ""
        cache: true
        name: root.identity?.displayName ?? ""
    }

    ColumnLayout {
        id: layout

        spacing: 0

        Layout.fillWidth: true

        Kirigami.Heading {
            level: 4
            font.pixelSize: Config.defaultFont.pixelSize + 2
            font.pointSize: -1
            text: root.identity?.displayNameHtml ?? ""
            type: Kirigami.Heading.Type.Primary
            color: Kirigami.Theme.textColor
            verticalAlignment: Text.AlignTop
            elide: Text.ElideRight
            textFormat: Text.StyledText
            maximumLineCount: 1

            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 0

            QQC2.Label {
                font.pixelSize: Config.defaultFont.pixelSize
                elide: Text.ElideRight
                text: `@${root.identity?.account ?? ""}`
                verticalAlignment: Text.AlignTop
                maximumLineCount: 1

                Layout.fillWidth: true
            }

            QQC2.Label {
                id: emailHeading
                font.pixelSize: Config.defaultFont.pixelSize
                text: root.ip ? " • " + root.ip : ""
                color: Kirigami.Theme.disabledTextColor
                visible: admin && root.ip
                verticalAlignment: Text.AlignTop
                elide: Text.ElideRight
                maximumLineCount: 1

                Layout.fillWidth: true
            }
        }
    }
}
