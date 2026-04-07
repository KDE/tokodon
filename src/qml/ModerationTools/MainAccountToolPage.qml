// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.tokodon
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as KirigamiComponents

FormCard.FormCardPage {
    id: root

    property var identity
    property int index
    property var model
    //moderation tool only visible if the position value of the account is greater or the account is not local
    readonly property bool displayModerationTool: (root.model.selectedAccountPosition > root.identity.position) || !root.identity.isLocal
    readonly property string lastActive: if (!isNaN(root.identity.lastActive)) {
        const today = new Date();
        const lastActive = root.identity.lastActive;
        if (lastActive.setHours(0,0,0,0) == today.setHours(0,0,0,0)) {
            return root.identity.lastActive.toLocaleTimeString(Qt.locale(), Locale.ShortFormat);
        } else {
            return root.identity.lastActive.toLocaleDateString();
        }
    } else {
        return i18n("Not Available");
    }
    readonly property bool largeScreen: width > Kirigami.Units.gridUnit * 25

    title: root.identity.userLevelIdentity.account

    FormCard.FormCardDialog {
        id: actionDialog

        title: i18n("Perform moderation action on %1", root.identity.userLevelIdentity.account)
        parent: root.QQC2.Overlay.overlay

        FormCard.FormRadioDelegate {
            id: warning
            visible: root.identity.isLocal
            text: i18nc("@info: Use this to send a warning to the user, without triggering any other action.", "Warn")
        }
        FormCard.FormRadioDelegate {
            id: freeze
            text: i18nc("@info: Prevent the user from using their account, but do not delete or hide their contents.", "Freeze")
            visible: !root.identity.disabled && root.identity.isLocal
        }
        FormCard.FormRadioDelegate {
            id: sensitive
            text: i18nc("@info: Force all this user's media attachments to be flagged as sensitive.", "Force-Sensitive")
            visible: !root.identity.sensitized
        }
        FormCard.FormRadioDelegate {
            id: limit
            text: i18nc("@info: Prevent the user from being able to post with public visibility, hide their posts and notifications from people not following them.", "Limit")
            visible: !root.identity.silenced
        }
        FormCard.FormRadioDelegate {
            id: suspend
            text: i18nc("@info: Prevent any interaction from or to this account and delete its contents. Revertible within 30 days.", "Suspend")
            visible: !root.identity.suspended
        }
        FormCard.FormRadioDelegate {
            id: emailWarning
            text: i18nc("@info: The user will receive an explanation of what happened with their account", "Notify the user per e-mail")
            checked: root.identity.isLocal
            visible: root.identity.isLocal
        }
        FormCard.FormTextFieldDelegate {
            id: message
            label: i18nc("@info: Send a warning note to the user.", "Custom warning")
            clip: true
            visible: root.identity.isLocal
        }

        standardButtons: FormCard.FormCardDialog.Ok | FormCard.FormCardDialog.Cancel

        Component.onCompleted: {
            const submitButton = standardButton(FormCard.FormCardDialog.Ok);
            submitButton.text = i18nc("@info:Button to submit the action aginst the user.", "Submit");
            submitButton.icon.name = "answer-symbolic";
        }

        onAccepted: {
            let action = ""
            if (warning.checked) {
                action = "none"
            } else if (freeze.checked) {
                action = "disable"
            } else if (sensitive.checked) {
                action = "sensitive"
            } else if (limit.checked) {
                action = "silence"
            } else if (suspend.checked) {
                action = "suspend"
            }
            root.model.actionAgainstAccount(root.index, action, emailWarning.checked, message.text)
            showPassiveNotification(i18n("Action taken successfully"))
            pageStack.layers.pop()
        }

        onRejected: actionDialog.close();
    }

    KirigamiComponents.MessageDialog {
        id: promptDialog

        property var actionName

        dialogType: KirigamiComponents.MessageDialog.Warning
        title: i18n("Are you sure?")
        subtitle: i18n("Action will be taken against the account.")
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        onAccepted: {
            let successMessage = ""
            switch (actionName) {
                case "approve":
                    root.model.approveAccount(root.index)
                    successMessage = i18n("Successfully approved %1's sign-up application", root.identity.userLevelIdentity.username);
                    break
                case "reject":
                    root.model.rejectAccount(root.index)
                    successMessage = i18n("Successfully rejected %1's sign-up application", root.identity.userLevelIdentity.username);
                    break
                case "unfreeze":
                    root.model.enableAccount(root.index)
                    successMessage = i18n("Successfully unfreezed %1's account", root.identity.userLevelIdentity.username);
                    break
                case "unsilence":
                    root.model.unsilenceAccount(root.index)
                    successMessage = i18n("Successfully unsilenced %1's account", root.identity.userLevelIdentity.username);
                    break
                case "unsuspend":
                    root.model.unsuspendAccount(root.index)
                    successMessage = i18n("Successfully unsuspended %1's account", root.identity.userLevelIdentity.username);
                    break
                case "unsensitive":
                    root.model.unsensitiveAccount(root.index)
                    successMessage = i18n("Successfully undo force-sensitive %1's account", root.identity.userLevelIdentity.username);
                    break
                case "deleteAccount":
                    root.model.deleteAccountData(root.index)
                    successMessage = i18n("Successfully deleted %1's account data", root.identity.userLevelIdentity.username);
                    break
                default:
                    break
            }
            showPassiveNotification(successMessage)
            pageStack.layers.pop()
        }
    }

    AccountHeader {
        id: header

        identity: root.identity.userLevelIdentity
        isSelf: false
        largeScreen: accountInfo.largeScreen
        canExcludeBoosts: false
        excludeBoosts: false
        showTabs: false
        Layout.fillWidth: true
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        background: Rectangle {
            id: view

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            color: Kirigami.ColorUtils.linearInterpolation(header.Kirigami.Theme.backgroundColor, view.Kirigami.Theme.backgroundColor, 0.5);
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 2
        visible: root.identity.inviteRequest

        FormCard.FormTextDelegate {
            text: i18n("Reasons for joining")

            description: root.identity.inviteRequest
            descriptionItem.textFormat: TextEdit.RichText
            onLinkActivated: (link) => Qt.openUrlExternally(link)
        }
    }

    FormCard.FormGridContainer {
        id: container

        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true

        infoCards: [
            FormCard.FormGridContainer.InfoCard {
                title: i18nc("@info Role of the account on this server.", "Role")
                subtitle: root.identity.userLevelIdentity.role ? root.identity.userLevelIdentity.role : i18n("No role")
            },
            FormCard.FormGridContainer.InfoCard {
                title: i18nc("@info The last time the account was active.", "Last Active")
                subtitle: root.lastActive
            },
            FormCard.FormGridContainer.InfoCard {
                title: i18nc("@info The current login status of the account.", "Login Status")
                subtitle: root.identity.loginStatus
            },
            FormCard.FormGridContainer.InfoCard {
                visible: root.identity.locale
                title: i18nc("@info: Locale of the user.", "Account Locale")
                subtitle: root.identity.locale
            },
            FormCard.FormGridContainer.InfoCard {
                visible: root.identity.email
                title: i18nc("@info: Email of the user.", "Email")
                subtitle: root.identity.email
            },
            FormCard.FormGridContainer.InfoCard {
                visible: root.identity.email
                title: i18nc("@info: Email confirmation status of the user.","Email Status")
                subtitle: root.identity.emailStatus ? i18n("Confirmed") : i18n("Not Confirmed")
            },
            FormCard.FormGridContainer.InfoCard {
                visible: root.identity.joined
                title: i18nc("@info: Joining date of the user","Joined")
                subtitle: root.identity.joined.toLocaleDateString()
            },
            FormCard.FormGridContainer.InfoCard {
                visible: root.identity.invitedByIdentity
                title: i18n("Invited By")
                subtitle: root.identity.invitedByIdentity?.username ?? ""
            }
        ]
    }

    FormCard.FormHeader {
        visible: root.identity.ips.length > 0
        title: i18n("Most recent IP")
    }

    FormCard.FormCard {
        visible: root.identity.ips.length > 0

        Repeater {
            model: root.identity.ips

            FormCard.FormTextDelegate {
                id: ipDelegate
                text: modelData.ip
            }
        }
    }

    footer: QQC2.ToolBar {
        visible: displayModerationTool
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18nc("@info: Approve the user's request", "Approve")
                icon.name: 'approved'
                visible: !root.identity.approved && root.identity.isLocal
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "approve"
                    promptDialog.open()
                }

            }

            QQC2.Button {
                text: i18nc("@info: Reject the user's request", "Reject")
                icon.name: 'cards-block'
                visible: !root.identity.approved && root.identity.isLocal
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "reject"
                    promptDialog.open()
                }
            }

            QQC2.Button {
                text: i18n("Delete Account Data")
                icon.name: 'delete'
                visible: root.identity.suspended
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "deleteAccount"
                    promptDialog.open()
                }
            }

            //enable a currently disabled account (freeze=disable)
            QQC2.Button {
                text: i18nc("@info: Unfreeze the user's account", "Unfreeze")
                visible: root.identity.disabled && !root.identity.suspended
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "unfreeze"
                    promptDialog.open()
                }
            }

            QQC2.Button {
                text: i18n("Undo Limit")
                visible: root.identity.silenced && !root.identity.suspended
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "unsilence"
                    promptDialog.open()
                }
            }

            QQC2.Button {
                text: i18n("Undo Suspension")
                visible: root.identity.suspended
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "unsuspend"
                    promptDialog.open()
                }
            }

            QQC2.Button {
                text: i18n("Undo force-sensitive")
                visible: root.identity.sensitized && !root.identity.suspended
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    promptDialog.actionName = "unsensitive"
                    promptDialog.open()
                }
            }

            //take action against the account
            QQC2.Button {
                text: i18n("Take action against this account")
                icon.name: 'cards-block'
                visible: (root.identity.approved || !root.identity.isLocal) && !root.identity.suspended
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    actionDialog.open()
                }
            }
        }
    }
}
