// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Privacy Policy")

    readonly property PrivacyPolicy backend: PrivacyPolicy {
        account: AccountManager.selectedAccount
    }

    Kirigami.PlaceholderMessage {
        text: i18nc("@info:placeholder", "No Privacy Policy")
        visible: backend.content.length === 0

        Layout.fillWidth: true
        Layout.preferredWidth: root.width - Kirigami.Units.gridUnit * 4
        Layout.fillHeight: true
        Layout.preferredHeight: root.height
    }

    FormCard.FormCard {
        visible: backend.content.length > 0

        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        FormCard.FormTextDelegate {
            text: i18nc("@info Last updated: relative date and time (e.g. 2 years ago)", "Last updated: %1", backend.updatedAt)
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            background: null
            contentItem: Kirigami.SelectableLabel {
                text: backend.content
            }
        }
    }
}
