// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as Components

import org.kde.tokodon

MastoPage {
    title: i18nc("@title:window", "Debug")

    FormCard.FormHeader {
        title: i18nc("@title:group", "Notifications")
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: i18n("Mention")
            onClicked: AccountManager.selectedAccount.mentionNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Favorite")
            onClicked: AccountManager.selectedAccount.favoriteNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Boost")
            onClicked: AccountManager.selectedAccount.boostNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Follow")
            onClicked: AccountManager.selectedAccount.followNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Follow Request")
            onClicked: AccountManager.selectedAccount.followRequestNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Status")
            onClicked: AccountManager.selectedAccount.statusNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Update/Edit")
            onClicked: AccountManager.selectedAccount.updateNotification()
        }

        FormCard.FormButtonDelegate {
            text: i18n("Poll")
            onClicked: AccountManager.selectedAccount.pollNotification()
        }
    }
}
