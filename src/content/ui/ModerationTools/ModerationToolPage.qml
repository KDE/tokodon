// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import org.kde.kirigamiaddons.settings 1.0 as KirigamiSettings
import QtQuick.Layouts 1.15

KirigamiSettings.CategorizedSettings {
    objectName: "moderationToolPage"
    actions: [
        KirigamiSettings.SettingAction {
            actionName: "Accounts"
            text: i18nc("@title", "Accounts")
            icon.name: "user-identity"
            page: Qt.resolvedUrl("AccountToolPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "Federation"
            text: i18nc("@title", "Federation")
            icon.name: "cloudstatus"
            page: Qt.resolvedUrl("FederationToolPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "IpRules"
            text: i18nc("@title", "IP Rules")
            icon.name: "cards-block"
            page: Qt.resolvedUrl("IpRulePage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "EmailBlock"
            text: i18nc("@title", "Email Blocks")
            icon.name: "mail-message"
            page: Qt.resolvedUrl("EmailBlockToolPage.qml")
        }
    ]
}
