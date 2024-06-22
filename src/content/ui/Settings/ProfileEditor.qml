// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import QtQuick.Dialogs
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import ".."

FormCard.FormCardPage {
    id: root

    property var account

    readonly property ProfileEditorBackend backend : ProfileEditorBackend {
        account: root.account
        onSendNotification: applicationWindow().showPassiveNotification(message)
    }

    readonly property bool canEditProfile: !AccountManager.accountHasIssue(account)

    title: i18n("Edit Account")

    actions: [
        Kirigami.Action {
            text: i18n("Logout")
            icon.name: "im-kick-user"

            onTriggered: logoutPrompt.open()
        }
    ]

    data: [
        Component {
            id: openFileDialog
            FileDialog {
                signal chosen(string path)
                title: i18n("Please choose a file")
                currentFolder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
                onAccepted: chosen(selectedFile)
            }
        },
        Kirigami.PromptDialog {
            id: logoutPrompt

            title: i18nc("@title", "Logout")
            subtitle: i18nc("@label", "Are you sure you want to log out of %1?", account.identity.displayName)
            standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
            showCloseButton: false

            onAccepted: {
                AccountManager.removeAccount(delegate.account);
                if (!AccountManager.hasAccounts) {
                    root.Window.window.close();
                }
            }
        }
    ]

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        enabled: canEditProfile

        Rectangle {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 9
            Layout.fillWidth: true
            clip: true
            color: Kirigami.Theme.backgroundColor
            Kirigami.Theme.colorSet: Kirigami.Theme.View

            ProfileHeader {
                backgroundUrl: backend.backgroundUrl
                avatarUrl: backend.avatarUrl
                displayName: backend.displayNameHtml
                account: backend.account.identity.account
            }
        }

        FormCard.FormTextFieldDelegate {
            label: i18n("Display Name")
            text: backend.displayName
            onTextChanged: backend.displayName = text
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            background: null
            Layout.fillWidth: true

            onClicked: bioField.clicked()
            onActiveFocusChanged: if (activeFocus) {
                bioField.forceActiveFocus();
            }

            contentItem: ColumnLayout {
                QQC2.Label {
                    text: i18n("Bio")
                    Layout.fillWidth: true
                }
                QQC2.TextArea {
                    id: bioField
                    Layout.fillWidth: true
                    wrapMode: TextEdit.Wrap
                    text: backend.note
                    onTextChanged: backend.note = text
                    activeFocusOnTab: false

                    KeyNavigation.tab: nextItemInFocusChain(true)
                    KeyNavigation.priority: KeyNavigation.BeforeItem
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            id: headerDelegate

            background: null
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                QQC2.Label {
                    text: i18n("Header")
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    QQC2.RoundButton {
                        id: headerUpload
                        icon.name: 'cloud-upload'
                        property var fileDialog: null;
                        Layout.alignment: Qt.AlignHCenter
                        onClicked: {
                            if (fileDialog !== null) {
                                return;
                            }

                            fileDialog = openFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

                            fileDialog.onAccepted.connect(function() {
                                const receivedSource = headerUpload.fileDialog.selectedFile;
                                headerUpload.fileDialog = null;
                                if (!receivedSource) {
                                    return;
                                }
                                backend.backgroundUrl = receivedSource;
                            });
                            fileDialog.onRejected.connect(function() {
                                headerUpload.fileDialog = null;
                            });
                            fileDialog.open();
                        }
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        text: i18n("PNG, GIF or JPG. At most 2 MB. Will be downscaled to 1500x500px")
                        wrapMode: Text.WordWrap
                    }
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    visible: text.length > 0
                    color: Kirigami.Theme.negativeTextColor
                    text: backend.backgroundUrlError
                    wrapMode: Text.WordWrap
                }

                Kirigami.LinkButton {
                    text: i18n("Delete")
                    color: Kirigami.Theme.negativeTextColor
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    onClicked: backend.backgroundUrl = ''
                }
            }
        }

        FormCard.FormDelegateSeparator { below: headerDelegate }

        FormCard.AbstractFormDelegate {
            background: null
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                QQC2.Label {
                    text: i18n("Avatar")
                    Layout.fillWidth: true
                }

                RowLayout {
                    QQC2.RoundButton {
                        id: avatarUpload
                        icon.name: 'cloud-upload'
                        property var fileDialog: null;
                        Layout.alignment: Qt.AlignHCenter
                        onClicked: {
                            if (fileDialog !== null) {
                                return;
                            }

                            fileDialog = openFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

                            fileDialog.chosen.connect(function(receivedSource) {
                                avatarUpload.fileDialog = null;
                                if (!receivedSource) {
                                    return;
                                }
                                backend.avatarUrl = receivedSource;
                            });
                            fileDialog.onRejected.connect(function() {
                                avatarUpload.fileDialog = null;
                            });
                            fileDialog.open();
                        }
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        text: i18n("PNG, GIF or JPG. At most 2 MB. Will be downscaled to 400x400px")
                        wrapMode: Text.WordWrap
                    }
                }

                QQC2.Label {
                    visible: text.length > 0
                    Layout.fillWidth: true
                    text: backend.avatarUrlError
                    wrapMode: Text.WordWrap
                    color: Kirigami.Theme.negativeTextColor
                }

                Kirigami.LinkButton {
                    text: i18n("Delete")
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    color: Kirigami.Theme.negativeTextColor
                    onClicked: backend.avatarUrl = ''
                }
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true

        enabled: canEditProfile

        FormCard.FormCheckDelegate {
            text: i18n("Require approval of follow requests")
            checked: backend.locked
            onCheckedChanged: backend.locked = checked
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormCheckDelegate {
            text: i18n("This is a bot account")
            checked: backend.bot
            onCheckedChanged: backend.bot = checked
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormCheckDelegate {
            text: i18n("Suggest account to others")
            checked: backend.discoverable
            onCheckedChanged: backend.discoverable = checked
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            text: i18nc("@label Account preferences", "Mark uploaded media as sensitive by default")
            checked: AccountManager.selectedAccount.preferences.defaultSensitive
            onToggled: AccountManager.selectedAccount.preferences.defaultSensitive = checked
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18nc("@label Account preferences", "Default post language")
            description: Qt.locale(AccountManager.selectedAccount.preferences.defaultLanguage).nativeLanguageName

            onClicked: languageSelect.open()

            LanguageSelector {
                id: languageSelect

                onAboutToShow: {
                    const sourceIndex = listView.model.sourceModel.indexOfValue(AccountManager.selectedAccount.preferences.defaultLanguage);
                    listView.currentIndex = listView.model.mapFromSource(sourceIndex).row;
                }
                onCodeSelected: code => AccountManager.selectedAccount.preferences.defaultLanguage = code
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            Layout.fillWidth: true
            id: postVisibility
            text: i18nc("@label Account preferences", "Default post visibility")
            model: [
                i18nc("@item:inlistbox Default post visibility rule", "Public"),
                i18nc("@item:inlistbox Default post visibility rule", "Unlisted"),
                i18nc("@item:inlistbox Default post visibility rule", "Private")
            ]
            Component.onCompleted: currentIndex = AccountManager.selectedAccount.preferences.defaultVisibility
            onCurrentValueChanged: AccountManager.selectedAccount.preferences.defaultVisibility = currentIndex
        }
    }

    footer: QQC2.ToolBar {
        enabled: canEditProfile
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18n("Reset")
                icon.name: 'edit-reset'
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: backend.fetchAccountInfo()
            }

            QQC2.Button {
                text: i18n("Apply")
                icon.name: 'dialog-ok'
                enabled: backend.backgroundUrlError.length === 0 && backend.avatarUrlError.length === 0
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: backend.save()
            }
        }
    }
}
