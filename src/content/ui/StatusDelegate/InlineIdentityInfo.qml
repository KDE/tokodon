// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

RowLayout {
    id: root

    required property var identity
    required property bool secondary

    Layout.fillWidth: true
    spacing: Kirigami.Units.largeSpacing

    Kirigami.Avatar {
        Layout.alignment: Qt.AlignTop
        Layout.rowSpan: 5
        source: root.identity.avatarUrl
        cache: true
        actions.main: Kirigami.Action {
            tooltip: i18n("View profile")
            onTriggered: Navigation.openAccount(root.identity.id)
        }
        name: root.identity.displayName
    }

    ColumnLayout {
        id: layout

        Layout.fillWidth: true
        Layout.bottomMargin: Kirigami.Units.smallSpacing

        spacing: 0

        Kirigami.Heading {
            level: 5
            text: root.identity.displayNameHtml
            type: Kirigami.Heading.Type.Primary
            color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
            verticalAlignment: Text.AlignTop
            elide: Text.ElideRight
            textFormat: Text.RichText
        }

        Kirigami.Heading {
            level: 5
            Layout.fillWidth: true
            elide: Text.ElideRight
            color: Kirigami.Theme.disabledTextColor
            text: `@${root.identity.account}`
            verticalAlignment: Text.AlignTop
        }
    }
}
