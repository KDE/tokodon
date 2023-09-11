// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.CategorizedSettings {
    id: settingsPage

    actions: [
        KirigamiSettings.SettingAction {
            actionName: "appearance"
            text: i18n("Appearance")
            icon.name: "preferences-desktop-theme-global"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/tokodon/content/ui/Settings/AppearancePage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "accounts"
            text: i18n("Accounts")
            icon.name: "preferences-system-users"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/tokodon/content/ui/Settings/AccountsPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "sonnet"
            text: i18n("Spell Checking")
            icon.name: "tools-check-spelling"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/tokodon/content/ui/Settings/SonnetPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "proxy"
            text: i18n("Network Proxy")
            icon.name: "network-connect"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/tokodon/content/ui/Settings/NetworkProxyPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "about"
            text: i18n("About Tokodon")
            icon.name: "help-about"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/tokodon/content/ui/Settings/AboutPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "aboutkde"
            text: i18n("About KDE")
            icon.name: "kde"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/tokodon/content/ui/Settings/AboutKDEPage.qml")
        }
    ]
}
