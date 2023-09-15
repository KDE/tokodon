// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.tokodon
import ".."

FormCard.FormCard {
    id: preferencesCard

    QQC2.Label {
        id: preferenceLabel
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
        padding: Kirigami.Units.largeSpacing
        wrapMode: Text.WordWrap
        text: i18nc("@label Account preferences", "These preferences apply to the current account and are synced to other clients.")
    }

    FormCard.FormDelegateSeparator { below: preferenceLabel }

    FormCard.FormSwitchDelegate {
        text: i18nc("@label Account preferences", "Mark uploaded media as sensitive by default")
        checked: AccountManager.selectedAccount.preferences.defaultSensitive
        onToggled: AccountManager.selectedAccount.preferences.defaultSensitive = checked
    }

    FormCard.FormDelegateSeparator {}

    FormCard.AbstractFormDelegate {
        onClicked: languageSelect.clicked();
        contentItem: RowLayout {
            QQC2.Label {
                Layout.fillWidth: true

                text: i18nc("@label Account preferences", "Default post language")
            }
            LanguageSelector {
                id: languageSelect

                Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.preferences.defaultLanguage);
                onActivated: AccountManager.selectedAccount.preferences.defaultLanguage = model.getCode(currentIndex)
                activeFocusOnTab: false
            }
        }
    }

    FormCard.FormDelegateSeparator {}

    FormCard.FormComboBoxDelegate {
        Layout.fillWidth: true
        id: postVisibility
        text: i18nc("@label Account preferences", "Default post visibility")
        textRole: "display"
        valueRole: "display"
        model: ListModel {
            ListElement {
                display: "Public"
            }
            ListElement {
                display: "Unlisted"
            }
            ListElement {
                display: "Private"
            }
        }
        Component.onCompleted: currentIndex = AccountManager.selectedAccount.preferences.defaultVisibility
        onCurrentValueChanged: AccountManager.selectedAccount.preferences.defaultVisibility = currentIndex
    }
}
