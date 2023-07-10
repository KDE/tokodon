// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import QtQuick.Layouts 1.15

Kirigami.CategorizedSettings {
    objectName: "moderationToolPage"
    actions: [
        Kirigami.SettingAction {
            actionName: "Accounts"
            text: i18n("Accounts")
            icon.name: "user-identity"
            page: Qt.resolvedUrl("AccountToolPage.qml")
        },
        Kirigami.SettingAction {
            actionName: "Federation"
            text: i18n("Federation")
            icon.name: "cloudstatus"
            page: Qt.resolvedUrl("FederationToolPage.qml")
        }
    ]
}
