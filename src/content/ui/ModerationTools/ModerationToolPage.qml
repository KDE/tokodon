// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.settings 1 as KirigamiSettings
import QtQuick.Layouts

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
            actionName: "Reports"
            text: i18n("Reports")
            icon.name: "flag"
            page: Qt.resolvedUrl("ReportToolPage.qml")
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
