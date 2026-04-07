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

    required property PostContent postContent
    required property string spoilerText

    signal toggleNotice()

    padding: Kirigami.Units.largeSpacing

    activeFocusOnTab: true

    Accessible.role: Accessible.Button
    Accessible.name: i18nc("@info", "Content Notice")
    Accessible.description: root.spoilerText
    Accessible.onPressAction: toggleNotice()

    Keys.onSpacePressed: toggleNotice()

    contentItem: RowLayout {
        id: warningLayout
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            Layout.alignment: Qt.AlignVCenter
            source: "data-information"
        }

        QQC2.Label {
            id: spoilerTextLabel
            Layout.fillWidth: true
            text: i18n("<b>Content Notice</b><br /> %1", root.spoilerText)
            wrapMode: Text.Wrap
            font: Config.defaultFont
        }

        QQC2.Button {
            activeFocusOnTab: false
            text: root.postContent.visible ? i18n("Show Less") : i18n("Show More")
            icon.name: root.postContent.visible ? "view-hidden-symbolic" : "view-visible-symbolic"
            onClicked: root.toggleNotice()
        }
    }

    background: Rectangle {
        radius: Kirigami.Units.cornerRadius
        color: Kirigami.Theme.activeBackgroundColor
    }
}
