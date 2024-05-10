// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigamiaddons.settings 1 as KirigamiSettings

KirigamiSettings.ConfigurationsView {
    id: root

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "Accounts"
            text: i18nc("@title", "Accounts")
            icon.name: "user-identity"
            page: () => Qt.createComponent("org.kde.tokodon", "AccountToolPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "Reports"
            text: i18n("Reports")
            icon.name: "flag"
            page: () => Qt.createComponent("org.kde.tokodon", "ReportToolPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "Federation"
            text: i18nc("@title", "Federation")
            icon.name: "cloudstatus"
            page: () => Qt.createComponent("org.kde.tokodon", "FederationToolPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "IpRules"
            text: i18nc("@title", "IP Rules")
            icon.name: "cards-block"
            page: () => Qt.createComponent("org.kde.tokodon", "IpRulePage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "EmailBlock"
            text: i18nc("@title", "Email Blocks")
            icon.name: "mail-message"
            page: () => Qt.createComponent("org.kde.tokodon", "EmailBlockToolPage")
        }
    ]
}
