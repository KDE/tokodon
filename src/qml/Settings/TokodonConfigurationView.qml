// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.ki18n
import org.kde.tokodon
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.ConfigurationView {
    id: root

    required property TokodonApplication application

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "appearance"
            text: KI18n.i18n("Appearance")
            icon.name: "preferences-desktop-theme-global"
            page: () => Qt.createComponent("org.kde.tokodon", "AppearancePage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "notifications"
            text: KI18n.i18n("Notifications")
            icon.name: "preferences-desktop-notification"
            page: () => Qt.createComponent("org.kde.tokodon", "NotificationsPage")
            visible: AccountManager.hasAccounts
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "safety"
            text: KI18n.i18n("Safety")
            icon.name: "preferences-security"
            page: () => Qt.createComponent("org.kde.tokodon", "SafetyPage")
            visible: AccountManager.hasAccounts
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "accounts"
            text: KI18n.i18n("Accounts")
            icon.name: "preferences-system-users"
            page: () => Qt.createComponent("org.kde.tokodon", "AccountsPage")
            initialProperties: () => {
                return {
                    application: root.application
                };
            }
            visible: AccountManager.hasAccounts
        },
        KirigamiSettings.SpellcheckingConfigurationModule {},
        KirigamiSettings.ConfigurationModule {
            moduleId: "proxy"
            text: KI18n.i18n("Network Proxy")
            icon.name: "network-connect"
            page: () => Qt.createComponent("org.kde.tokodon", "NetworkProxyPage")
        },
        KirigamiSettings.ShortcutsConfigurationModule {
            application: root.application
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "errorlog"
            text: KI18n.i18n("Error Log")
            icon.name: "error-symbolic"
            page: () => Qt.createComponent("org.kde.tokodon", "ErrorLogPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "about"
            text: KI18n.i18n("About Tokodon")
            icon.name: "org.kde.tokodon"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage")
            category: KI18n.i18nc("@title:group", "About")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "aboutkde"
            text: KI18n.i18n("About KDE")
            icon.name: "kde"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDEPage")
            category: KI18n.i18nc("@title:group", "About")
        }
    ]
}
