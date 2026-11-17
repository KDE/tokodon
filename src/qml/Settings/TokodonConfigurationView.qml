// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.tokodon
import org.kde.kirigami.settings as KirigamiSettings

KirigamiSettings.SettingsViewController {
    id: root

    required property TokodonApplication application

    modules: [
        KirigamiSettings.SettingsModule {
            moduleId: "appearance"
            name: KI18n.i18n("Appearance")
            icon.name: "preferences-desktop-theme-global"
            page: AppearancePage {}
        },
        KirigamiSettings.SettingsModule {
            moduleId: "notifications"
            name: KI18n.i18n("Notifications")
            icon.name: "preferences-desktop-notification"
            page: NotificationsPage {}
            visible: AccountManager.hasAccounts
        },
        KirigamiSettings.SettingsModule {
            moduleId: "safety"
            name: KI18n.i18n("Safety")
            icon.name: "preferences-security"
            page: SafetyPage {}
            visible: AccountManager.hasAccounts
        },
        KirigamiSettings.SettingsModule {
            moduleId: "accounts"
            name: KI18n.i18n("Accounts")
            icon.name: "preferences-system-users"
            page: AccountsPage {}
            initialProperties: () => {
                return {
                    application: root.application
                };
            }
            visible: AccountManager.hasAccounts
        },
        KirigamiSettings.SpellcheckingSettingsModule {},
        KirigamiSettings.SettingsModule {
            moduleId: "proxy"
            name: KI18n.i18n("Network Proxy")
            icon.name: "network-connect"
            page: NetworkProxyPage {}
        },
        KirigamiSettings.ShortcutsSettingsModule {},
        KirigamiSettings.SettingsModule {
            moduleId: "errorlog"
            name: KI18n.i18n("Error Log")
            icon.name: "error-symbolic"
            page: ErrorLogPage {}
        },
        KirigamiSettings.SettingsModule {
            moduleId: "about"
            name: KI18n.i18n("About Tokodon")
            icon.name: "org.kde.tokodon"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage")
            category: KI18n.i18nc("@title:group", "About")
        },
        KirigamiSettings.SettingsModule {
            moduleId: "aboutkde"
            name: KI18n.i18n("About KDE")
            icon.name: "kde"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDEPage")
            category: KI18n.i18nc("@title:group", "About")
        }
    ]
}
