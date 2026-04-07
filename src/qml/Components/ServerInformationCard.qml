// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.tokodon

FormCard.FormCard {
    id: root

    required property string kind

    readonly property ServerInformation backend: ServerInformation {
        account: AccountManager.selectedAccount
        kind: root.kind
    }

    FormCard.FormPlaceholderMessageDelegate {
        icon.name: "question-symbolic"
        text: i18nc("@info:placeholder", "No Information")
        visible: backend.content.length === 0
    }

    FormCard.FormTextDelegate {
        text: i18nc("@info Last updated: relative date and time (e.g. 2 years ago)", "Last updated: %1", backend.updatedAt)
        visible: backend.content.length > 0
    }

    FormCard.FormDelegateSeparator {
        visible: backend.content.length > 0
    }

    FormCard.AbstractFormDelegate {
        visible: backend.content.length > 0
        background: null
        contentItem: Kirigami.SelectableLabel {
            text: backend.content

            onLinkActivated: link => applicationWindow().navigateLink(link, true)
        }
    }
}
