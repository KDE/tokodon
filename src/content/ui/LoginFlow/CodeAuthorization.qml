// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard

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
            onClicked: {
                root.setAuthCode(tokenField.text);
            }
        }
    }
}
