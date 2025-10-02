// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.PromptDialog {
    id: root

    required property var sourceIdentity
    required property var targetIdentity

    width: 400

    padding: Kirigami.Units.largeSpacing

    title: i18nc("@title", "Confirm Boost")
    standardButtons: Kirigami.Dialog.Cancel
    customFooterActions: [
        Kirigami.Action {
            text: i18nc("@action:button Boost this post", "Boost")
            icon.name: "boost"
            onTriggered: root.accept()
        }
    ]

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: root.title

            Layout.fillWidth: true
        }

        InlineIdentityInfo {
            id: sourceIdentityInfo
            identity: root.sourceIdentity

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }

        Kirigami.Icon {
            source: "boost"

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }

        InlineIdentityInfo {
            id: targetIdentityInfo
            identity: root.targetIdentity

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }
    }
}
