// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.tokodon

MastoPage {
    id: root

    title: i18nc("@title:window", "Login Issue")

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextDelegate {
            leading: Kirigami.Icon {
                source: "data-warning"
            }

            text: i18n("There was an issue logging into the server:<br><b>%1</b>", AccountManager.selectedAccountLoginIssue())
            textItem.textFormat: Text.RichText
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextDelegate {
            id: helpLabel
            text: i18n("Please check if the password service is running. For other login issues, you can try logging into the server's website.\n\nYou can attempt to log in again with the button below, or restart Tokodon.")
            textItem {
                wrapMode: Text.WordWrap
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: continueButton
            text: i18n("View Website")
            icon.name: "internet-services-symbolic"
            onClicked: Qt.openUrlExternally(AccountManager.selectedAccount.instanceUri)
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: attemptButton
            text: i18n("Re-Attempt Log In")
            icon.name: "edit-redo-symbolic"
            onClicked: {
                AccountManager.selectedAccount.validateToken()
                AccountManager.selectedAccount = AccountManager.selectedAccount; // reload the pages
            }
        }
    }
}
