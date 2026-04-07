// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.tokodon
import org.kde.kirigamiaddons.components as Components

import ".."

QQC2.Control {
    id: root

    required property var filters
    required property bool filtered

    readonly property string matchedFilters: root.filters.join(', ')

    signal toggleFilter

    padding: Kirigami.Units.largeSpacing

    activeFocusOnTab: true
    visible: root.filtered
    Accessible.role: Accessible.Button
    Accessible.name: i18nc("@info", "Filter")
    Accessible.description: matchedFilters
    Accessible.onPressAction: toggleFilter()

    Keys.onSpacePressed: toggleFilter()

    contentItem: RowLayout {
        id: warningLayout
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            Layout.alignment: Qt.AlignVCenter
            source: "view-filter"
        }

        QQC2.Label {
            id: spoilerTextLabel
            Layout.fillWidth: true
            text: i18n("<b>Filtered</b><br /> %1", root.matchedFilters)
            wrapMode: Text.Wrap
            font: Config.defaultFont
        }

        QQC2.Button {
            activeFocusOnTab: false
            text: i18nc("@action:button", "Show Anyway")
            icon.name: "view-visible-symbolic"
            onClicked: root.toggleFilter()
        }
    }

    background: Rectangle {
        radius: Kirigami.Units.cornerRadius
        color: Kirigami.Theme.activeBackgroundColor
    }
}
