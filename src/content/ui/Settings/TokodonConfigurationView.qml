// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma Singleton

import QtQuick
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.ConfigurationView {
    id: root

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "appearance"
            text: i18n("Appearance")
            icon.name: "preferences-desktop-theme-global"
            page: () => Qt.createComponent("org.kde.tokodon", "AppearancePage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "notifications"
            text: i18n("Notifications")
            icon.name: "preferences-desktop-notification"
            page: () => Qt.createComponent("org.kde.tokodon", "NotificationsPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "accounts"
            text: i18n("Accounts")
            icon.name: "preferences-system-users"
            page: () => Qt.createComponent("org.kde.tokodon", "AccountsPage")
        },
        KirigamiSettings.SpellcheckingConfigurationModule {},
        KirigamiSettings.ConfigurationModule {
            moduleId: "proxy"
            text: i18n("Network Proxy")
            icon.name: "network-connect"
            page: () => Qt.createComponent("org.kde.tokodon", "NetworkProxyPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "about"
            text: i18n("About Tokodon")
            icon.name: "org.kde.tokodon"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage")
            category: i18nc("@title:group", "About")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "aboutkde"
            text: i18n("About KDE")
            icon.name: "kde"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDE")
            category: i18nc("@title:group", "About")
        }
    ]
}
