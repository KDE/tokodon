// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

/**
 * Authorizing login with the account externally by the internet browser.
 */
BaseAuthorization {
    id: root

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        FormCard.FormButtonDelegate {
            text: i18n("Use Authorization Code")
            description: i18n("Use this login method if the above link does not work.")
            icon.name: "lock-symbolic"
            onClicked: loginPage.openCodeAuthPage()
        }
    }
}
