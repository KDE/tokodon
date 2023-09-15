// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

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
        FormCard.FormTextDelegate {
            id: errorLabel
            text: i18n("There was an issue when logging into the server: %1", AccountManager.selectedAccountLoginIssue)
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: continueButton
            text: i18n("Continue")
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "LoginPage"))
        }
    }
}
