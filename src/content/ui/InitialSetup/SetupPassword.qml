// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
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

    title: i18nc("@title:window", "Password Service")

    FormCard.FormHeader {
        title: i18n("Setup Required")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: i18n("Tokodon requires a password service to be running to save personally sensitive information.\n\nTokodon can use KWallet, GNOME Keyring or any libsecret-compatible service.\n\nAfter you installed the service, make sure it's running and restart Tokodon.")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: learnMoreButton
            text: i18n("Quit Tokodon")
            icon.name: "application-exit-symbolic"
            onClicked: Qt.exit(0)
        }
    }
}
