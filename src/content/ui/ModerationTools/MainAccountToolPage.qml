// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import Qt.labs.platform 1.1
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import QtGraphicalEffects 1.0

Kirigami.ScrollablePage {
    id: root
    property var identity
    property int index
    property var model
    //moderation tool only visible if the position value of the account is greater or the account is not local
    readonly property bool displayModerationTool: (root.model.selectedAccountPosition > root.identity.position) || !root.identity.isLocal

    title: root.identity.userLevelIdentity.account
    leftPadding: 0
    rightPadding: 0


    Kirigami.PromptDialog {
        id: actionDailog
        title: i18n("Perform moderation action on %1", root.identity.userLevelIdentity.account)
        mainItem: ColumnLayout {
            Layout.fillWidth: true
            QQC2.RadioButton {
                id: warning
                visible: root.identity.isLocal
                text: i18nc("@info: Use this to send a warning to the user, without triggering any other action.", "Warn")
            }
            QQC2.RadioButton {
                id: freeze
                text: i18nc("@info: Prevent the user from using their account, but do not delete or hide their contents.", "Freeze")
                visible: !root.identity.disabled && root.identity.isLocal
            }
            QQC2.RadioButton {
                id: sensitive
                text: i18nc("@info: Force all this user's media attachments to be flagged as sensitive.", "Force-Senstive")
                visible: !root.identity.sensitized
            }
            QQC2.RadioButton {
                id: limit
                text: i18nc("@info: Prevent the user from being able to post with public visibility, hide their posts and notifications from people not following them.", "Limit")
                visible: !root.identity.silenced
            }
            QQC2.RadioButton {
                id: suspend
                text: i18nc("@info: Prevent any interaction from or to this account and delete its contents. Revertible within 30 days.", "Suspend")
                visible: !root.identity.suspended
            }
            QQC2.CheckBox {
                id: emailWarning
                text: i18nc("@info: The user will receive an explanation of what happened with their account", "Notify the user per e-mail")
                checked: root.identity.isLocal
                visible: root.identity.isLocal
            }
            QQC2.TextField {
                id: message
                placeholderText: i18nc("@info: Send a warning note to the user.", "Custom warning")
                wrapMode: Text.Wrap
                clip: true
                visible: root.identity.isLocal
                Kirigami.SpellChecking.enabled: true
                Layout.fillWidth: true
            }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info:Button to submit the action aginst the user.", "Submit")
                icon.name: "answer"
                onTriggered: {
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
            },
            Kirigami.Action {
                text: i18nc("@info:Cancel button to close the dailog.", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: actionDailog.close();
            }
        ]
    }

    Kirigami.PromptDialog {
        id: promptDialog
        property var actionName
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

    ColumnLayout {
        id: layout

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                Rectangle {
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 9
                    Layout.fillWidth: true
                    clip: true
                    color: Kirigami.Theme.backgroundColor
                    Kirigami.Theme.colorSet: Kirigami.Theme.View

                    Image {
                        id: bg
                        anchors.centerIn: parent
                        source: root.identity.userLevelIdentity.backgroundUrl
                        fillMode: Image.PreserveAspectFit
                        visible: true //change
                    }

                    QQC2.Pane {
                        id: pane
                        visible: true
                        background: Item {
                            // Background image

                            FastBlur {
                                id: blur
                                source: bg
                                radius: 48
                                width: pane.width
                                height: pane.height
                            }
                            ColorOverlay {
                                width: pane.width
                                height: pane.height
                                source: blur
                                color: "#66808080"
                            }
                            Rectangle {
                                id: strip
                                color: "#66F0F0F0"
                                anchors.bottom: parent.bottom;
                                height: 2 * Kirigami.Units.gridUnit
                                width: parent.width
                                visible: children.length > 0
                            }
                        }
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom;
                        anchors.right: parent.right
                        contentItem:
                            ColumnLayout {
                            RowLayout {
                                implicitHeight: Kirigami.Units.gridUnit * 5
                                Kirigami.Avatar {
                                    source: root.identity.userLevelIdentity.avatarUrl
                                }

                                Column {
                                    Layout.fillWidth: true
                                    Kirigami.Heading {
                                        level: 5
                                        text: root.identity.userLevelIdentity.username
                                        type: Kirigami.Heading.Primary
                                    }
                                    QQC2.Label {
                                        text: "@" + root.identity.userLevelIdentity.account
                                    }
                                }
                            }
                        }
                    }
                }

                Repeater {
                    model: root.identity.userLevelIdentity.fields
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Kirigami.Separator {
                            Layout.fillWidth: true
                        }
                        RowLayout {
                            spacing: 0
                            QQC2.Pane {
                                contentItem: QQC2.Label {
                                    text: modelData.name
                                    wrapMode: Text.WordWrap
                                }
                                Layout.minimumWidth: Kirigami.Units.gridUnit * 10
                                Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                                Kirigami.Theme.colorSet: Kirigami.Theme.View
                                leftPadding: Kirigami.Units.largeSpacing
                                rightPadding: Kirigami.Units.largeSpacing
                                bottomPadding: 0
                                topPadding: 0
                            }

                            QQC2.TextArea {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                readOnly: true
                                wrapMode: Text.WordWrap
                                background: Rectangle {
                                    color: modelData.verified_at !== null ? Kirigami.Theme.positiveBackgroundColor : Kirigami.Theme.backgroundColor
                                }
                                textFormat: TextEdit.RichText
                                text: modelData.value
                                onLinkActivated: Qt.openUrlExternally(link)
                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                                }
                            }
                        }
                    }
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }
                RowLayout {
                    visible: root.identity.userLevelIdentity.bio
                    spacing: 0
                    QQC2.Pane {
                        contentItem: QQC2.Label {
                            text: i18nc("@info Bio label of account.","Bio")
                            wrapMode: Text.WordWrap
                        }
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 10
                        Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                        Kirigami.Theme.colorSet: Kirigami.Theme.View
                        leftPadding: Kirigami.Units.largeSpacing
                        rightPadding: Kirigami.Units.largeSpacing
                        bottomPadding: 0
                        topPadding: 0
                    }

                    QQC2.TextArea {
                        text: root.identity.userLevelIdentity.bio
                        textFormat: TextEdit.RichText
                        readOnly: true
                        Layout.fillWidth: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        Layout.topMargin: Kirigami.Units.smallSpacing
                        Layout.bottomMargin: Kirigami.Units.smallSpacing
                        leftPadding: 0
                        rightPadding: 0
                        bottomPadding: 0
                        topPadding: 0
                        background: null
                        wrapMode: Text.WordWrap
                        onLinkActivated: Qt.openUrlExternally(link)
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                        }
                    }

                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }
                RowLayout {
                    visible: root.identity.inviteRequest
                    spacing: 0
                    QQC2.Pane {
                        contentItem: QQC2.Label {
                            text: i18n("Reasons for joining")
                            wrapMode: Text.WordWrap
                        }
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 10
                        Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                        Kirigami.Theme.colorSet: Kirigami.Theme.View
                        leftPadding: Kirigami.Units.largeSpacing
                        rightPadding: Kirigami.Units.largeSpacing
                        bottomPadding: 0
                        topPadding: 0
                    }

                    QQC2.TextArea {
                        text: root.identity.inviteRequest
                        textFormat: TextEdit.RichText
                        readOnly: true
                        Layout.fillWidth: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        Layout.topMargin: Kirigami.Units.smallSpacing
                        Layout.bottomMargin: Kirigami.Units.smallSpacing
                        leftPadding: 0
                        rightPadding: 0
                        bottomPadding: 0
                        topPadding: 0
                        background: null
                        wrapMode: Text.WordWrap
                        onLinkActivated: Qt.openUrlExternally(link)
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                        }
                    }
                }
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            id: grid
            readonly property int cellWidth: Kirigami.Units.gridUnit * 10
            readonly property int cellHeight: Math.max(vaults.implicitHeight,
                                                       activities.implicitHeight,
                                                       kdeconnect.implicitHeight,
                                                       overview.implicitHeight,
                                                       krunner.implicitHeight,
                                                       ghns.implicitHeight)

            Layout.fillWidth: true

            columns: 3
            columnSpacing: Kirigami.Units.smallSpacing
            rowSpacing: Kirigami.Units.smallSpacing

            // First row
            AccountInfoButton {
                id: vaults
                Layout.preferredWidth: grid.cellWidth
                Layout.preferredHeight: grid.cellHeight
                title:  root.identity.userLevelIdentity.statusesCount
                subtitle: i18nc("@info:Number of Posts", "Posts")
            }
            AccountInfoButton {
                id: activities
                Layout.preferredWidth: grid.cellWidth
                Layout.preferredHeight: grid.cellHeight
                title: root.identity.userLevelIdentity.followersCount
                subtitle: i18nc("@info:Number of followers.", "Followers")
            }
            AccountInfoButton {
                id: kdeconnect
                Layout.preferredWidth: grid.cellWidth
                Layout.preferredHeight: grid.cellHeight
                title: root.identity.userLevelIdentity.followingCount
                subtitle: i18nc("@info:row Number of accounts followed by the account", "Following")
            }

            // Second row
            AccountInfoButton {
                id: krunner
                Layout.preferredWidth: grid.cellWidth
                Layout.preferredHeight: grid.cellHeight
                title: root.identity.lastActive ? root.identity.lastActive : "Not Available"
                subtitle: i18nc("@info The last time the account was active.", "Last Active")
            }
            AccountInfoButton {
                id: overview
                Layout.preferredWidth: grid.cellWidth
                Layout.preferredHeight: grid.cellHeight
                title: root.identity.userLevelIdentity.role ? root.identity.userLevelIdentity.role : "No role"
                subtitle: i18nc("@info Role of the account on this server.", "Role")
            }
            AccountInfoButton {
                id: ghns
                Layout.preferredWidth: grid.cellWidth
                Layout.preferredHeight: grid.cellHeight
                title: root.identity.loginStatus
                subtitle: i18nc("@info The current login status of the account.", "Login Status")
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: root.identity.email
            contentItem: ColumnLayout {
                spacing: 0
                RowLayout {
                    visible: root.identity.role
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: "Role"
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    QQC2.Label {
                        text: root.identity.role
                    }
                }
                MobileForm.FormDelegateSeparator {}
                RowLayout {
                    visible: root.identity.email
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18nc("@info: Email of the user.", "Email")
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    QQC2.Label {
                        text: root.identity.email
                    }
                }
                MobileForm.FormDelegateSeparator {}
                RowLayout {
                    visible: root.identity.emailStatus
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18nc("@info: Email confirmation status of the user.","Email Status")
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    QQC2.Label {
                        text: root.identity.emailStatus ? "Confirmed" : "Not Confirmed"
                    }
                }
                MobileForm.FormDelegateSeparator {}
                RowLayout {
                    visible: root.identity.locale
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18nc("@info: Locale of the user.", "Account Locale")
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    QQC2.Label {
                        text: root.identity.locale
                    }
                }
                MobileForm.FormDelegateSeparator {}
                RowLayout {
                    visible: root.identity.joined
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18nc("@info: Joining date of the user","Joined")
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    QQC2.Label {
                        text: root.identity.joined
                    }
                }
                MobileForm.FormDelegateSeparator {}
                RowLayout {
                    visible: root.identity.ips
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18n("Most recent IP")
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Repeater {
                            model: root.identity.ips
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 0
                                QQC2.Label {
                                    text: modelData.ip
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.topMargin: Kirigami.Units.smallSpacing
                                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                                }
                                MobileForm.FormDelegateSeparator {}
                            }
                        }
                    }
                }
                MobileForm.FormDelegateSeparator {}
                RowLayout {
                    visible: root.identity.invitedByIdentity
                    spacing: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing * 2
                    Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18n("Invited By")
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.textColor
                    }
                    RowLayout {
                        spacing: Kirigami.Units.largeSpacing
                        Kirigami.Avatar {
                            source: root.identity.invitedByIdentity ? root.identity.invitedByIdentity.avatarUrl : ""
                            implicitHeight: Kirigami.Units.gridUnit
                            implicitWidth: implicitHeight
                        }
                        QQC2.Label {
                            text: root.identity.invitedByIdentity ? root.identity.invitedByIdentity.username : ""
                        }
                    }
                }
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
                    actionDailog.open()
                }
            }
        }
    }
}
