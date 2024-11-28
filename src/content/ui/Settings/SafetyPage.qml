// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml
import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.tokodon
import org.kde.tokodon.private

FormCard.FormCardPage {
    id: root

    property var account: AccountManager.selectedAccount
    readonly property var config: account.config

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        FormCard.FormButtonDelegate {
            icon.name: "microphone-sensitivity-muted"
            text: i18n("Muted Users")
            onClicked: root.Window.window.pageStack.push(socialGraphComponent, { name: "mutes" })
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            icon.name: "cards-block"
            text: i18n("Blocked Users")
            onClicked: root.Window.window.pageStack.push(socialGraphComponent, { name: "blocks" })
        }
    }
}
