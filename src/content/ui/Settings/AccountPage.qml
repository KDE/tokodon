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

Kirigami.Page {
    id: root

    property AbstractAccount account

    readonly property ProfileEditorBackend backend : ProfileEditorBackend {
        account: root.account
        onSendNotification: applicationWindow().showPassiveNotification(message)
    }

    readonly property bool canEditProfile: !AccountManager.accountHasIssue(account)

    title: i18n("Edit Account")
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

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
                AccountManager.removeAccount(root.account);
                QQC2.ApplicationWindow.window.pageStack.pop();
                if (!AccountManager.hasAccounts) {
                    root.Window.window.close();
                }
            }
        }
    ]

    property Kirigami.Action profileAction: Kirigami.Action {
        text: i18n("Profile")
        icon.name: "user-group-properties"
        checkable: true
        checked: true
    }

    property Kirigami.Action accountAction: Kirigami.Action {
        text: i18n("Account")
        icon.name: "configure-symbolic"
        checkable: true
    }

    header: Kirigami.NavigationTabBar {
        anchors.left: parent.left
        anchors.right: parent.right
        actions: [profileAction, accountAction]

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
    }

    StackLayout {
        anchors.fill: parent

        currentIndex : accountAction.checked ? 1 : 0

        QQC2.ScrollView {
            clip: true
            topPadding: 0
            leftPadding: 0
            rightPadding: 0
            bottomPadding: 0

            ProfileEditor {
                id: editor

                anchors.fill: parent
                account: root.account
            }
        }
        QQC2.ScrollView {
            clip: true
            topPadding: 0
            leftPadding: 0
            rightPadding: 0
            bottomPadding: 0

            ColumnLayout {
                spacing: 0
                anchors.fill: parent

                FormCard.FormCard {
                    Layout.topMargin: Kirigami.Units.largeSpacing * 4
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 4
                    Layout.fillWidth: true

                    enabled: canEditProfile

                    FormCard.FormCheckDelegate {
                        text: i18n("Require approval for new followers")
                        description: i18n("By default new followers are automatically accepted. Uncheck if you want to manually approve or deny new ones. You always have the option to force someone to unfollow you.")
                        checked: backend.locked
                        onCheckedChanged: backend.locked = checked
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormCheckDelegate {
                        text: i18n("Is automated")
                        description: i18n("Whether to publicly mark this account as doing any kind of automated actions.")
                        checked: backend.bot
                        onCheckedChanged: backend.bot = checked
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormCheckDelegate {
                        text: i18n("Feature profile and posts")
                        description: i18n("Your public profile and posts may be featured to other users.")
                        checked: backend.discoverable
                        onCheckedChanged: backend.discoverable = checked
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormCheckDelegate {
                        text: i18n("Publicly list follows and followers")
                        description: i18n("By default everyone you follow and everyone who follows you is public.")
                        checked: !root.account.preferences.hideCollections
                        onCheckedChanged: root.account.preferences.hideCollections = !checked
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormSwitchDelegate {
                        text: i18nc("@label Account preferences", "Mark uploaded media as sensitive by default")
                        checked: root.account.preferences.defaultSensitive
                        onToggled: root.account.preferences.defaultSensitive = checked
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormSwitchDelegate {
                        text: i18nc("@label Account preferences", "Include public posts in search results")
                        checked: root.account.preferences.indexable
                        onToggled: root.account.preferences.indexable = checked
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormButtonDelegate {
                        text: i18nc("@label Account preferences", "Default post language")
                        description: Qt.locale(root.account.preferences.defaultLanguage).nativeLanguageName

                        onClicked: languageSelect.createObject().open()

                        Component {
                            id: languageSelect

                            LanguageSelector {
                                parent: root.QQC2.Overlay.overlay
                                onAboutToShow: {
                                    const sourceIndex = listView.model.sourceModel.indexOfValue(root.account.preferences.defaultLanguage);
                                    listView.currentIndex = listView.model.mapFromSource(sourceIndex).row;
                                }
                                onCodeSelected: code => root.account.preferences.defaultLanguage = code
                                onClosed: destroyed()
                            }
                        }
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormComboBoxDelegate {
                        Layout.fillWidth: true
                        id: postVisibility
                        text: i18nc("@label Account preferences", "Default post visibility")
                        model: [
                            i18nc("@item:inlistbox Default post visibility rule", "Public"),
                            i18nc("@item:inlistbox Default post visibility rule", "Unlisted"),
                            i18nc("@item:inlistbox Default post visibility rule", "Private")
                        ]
                        Component.onCompleted: currentIndex = root.account.preferences.defaultVisibility
                        onCurrentValueChanged: root.account.preferences.defaultVisibility = currentIndex
                    }

                    FormCard.FormDelegateSeparator {
                    }

                    FormCard.FormLinkDelegate {
                        Layout.fillWidth: true
                        id: websiteDelegate
                        icon.name: "globe-symbolic"
                        text: i18nc("@abel Account preferences", "Open Server in Browser")
                        description: i18n("Some settings can only be configured on your server's website.")
                        url: root.account.instanceUri
                    }
                }
            }
        }
    }

    footer: editor.profileFooter
}
