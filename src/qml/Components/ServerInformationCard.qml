// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

FormCard.FormCard {
    autoSeparators: true
    id: root

    required property string kind

    readonly property ServerInformation backend: ServerInformation {
        account: AccountManager.selectedAccount
        kind: root.kind
    }

    FormCard.FormPlaceholderMessageDelegate {
        icon.name: "question-symbolic"
        text: KI18n.i18nc("@info:placeholder", "No Information")
        visible: root.backend.content.length === 0
    }

    FormCard.FormTextDelegate {
        text: KI18n.i18nc("@info Last updated: relative date and time (e.g. 2 years ago)", "Last updated: %1", root.backend.updatedAt)
        visible: root.backend.content.length > 0
    }

    FormCard.AbstractFormDelegate {
        visible: root.backend.content.length > 0
        background: null
        contentItem: Kirigami.SelectableLabel {
            text: root.backend.content

            onLinkActivated: link => MainWindow.instance.navigateLink(link, true)
        }
    }
}
