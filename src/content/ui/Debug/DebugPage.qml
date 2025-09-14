// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.tokodon

MastoPage {
    title: "Debug"

    FormCard.FormHeader {
        title: "Alerts"
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Increase follow request count")
            onClicked: AccountManager.selectedAccount.increaseFollowRequests()
        }
        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Decrease follow request count")
            onClicked: AccountManager.selectedAccount.decreaseFollowRequests()
        }
    }

    FormCard.FormHeader {
        title: "Notifications"
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            text: "Mention"
            onClicked: AccountManager.selectedAccount.mentionNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Favorite"
            onClicked: AccountManager.selectedAccount.favoriteNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Boost"
            onClicked: AccountManager.selectedAccount.boostNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Follow"
            onClicked: AccountManager.selectedAccount.followNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Follow Request"
            onClicked: AccountManager.selectedAccount.followRequestNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Status"
            onClicked: AccountManager.selectedAccount.statusNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Update/Edit"
            onClicked: AccountManager.selectedAccount.updateNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Poll"
            onClicked: AccountManager.selectedAccount.pollNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Annual Report"
            onClicked: AccountManager.selectedAccount.annualReportNotification()
        }

        FormCard.FormButtonDelegate {
            text: "Unknown/Malformed"
            onClicked: AccountManager.selectedAccount.unknownNotification()
        }
    }
}
