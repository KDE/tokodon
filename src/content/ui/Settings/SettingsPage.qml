// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

Kirigami.CategorizedSettings {
    id: settingsPage
    objectName: "settingsPage"
    actions: [
        Kirigami.SettingAction {
            text: i18n("Accounts")
            icon.name: "preferences-system-users"
            page: Qt.resolvedUrl("AccountsPage.qml")
        },
        Kirigami.SettingAction {
            text: i18n("Spell Checking")
            iconName: "tools-check-spelling"
            page: Qt.resolvedUrl("SonnetConfigPage.qml")
        }
    ]
}
