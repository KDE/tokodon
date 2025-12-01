// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

/**
 * Authorizing login with the account via an auth code.
 */
BaseAuthorization {
    id: root

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        FormCard.FormTextFieldDelegate {
            id: tokenField
            label: i18n("Enter authorization token:")
            onAccepted: continueButton.clicked()
        }

        FormCard.FormDelegateSeparator { above: tokenField }

        FormCard.FormButtonDelegate {
            text: i18n("Continue")
            enabled: tokenField.text !== ""
            onClicked: {
                root.setAuthCode(tokenField.text);
            }
        }
    }
}
