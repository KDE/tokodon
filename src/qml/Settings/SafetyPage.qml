// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Window
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    property AbstractAccount account: AccountManager.selectedAccount
    readonly property var config: account.config

    FormCard.FormCard {
        autoSeparators: true
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        FormCard.FormButtonDelegate {
            icon.name: "filter-symbolic"
            text: KI18n.i18nc("@action:button", "Filters")
            onClicked: (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "FiltersPage"))
        }

        FormCard.FormButtonDelegate {
            icon.name: "microphone-sensitivity-muted"
            text: KI18n.i18n("Muted Users")
            onClicked: (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "SocialGraphPage"), { name: "mutes" })
        }

        FormCard.FormButtonDelegate {
            icon.name: "cards-block"
            text: KI18n.i18n("Blocked Users")
            onClicked: (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "SocialGraphPage"), { name: "blocks" })
        }

        FormCard.FormButtonDelegate {
            icon.name: "internet-services-symbolic"
            text: KI18n.i18n("Blocked Domains")
            onClicked: (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "BlockedDomainsPage"))
        }
    }
}
