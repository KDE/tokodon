// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.tokodon

import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as KirigamiComponents
import "../PostDelegate"

FormCard.FormCardPage {
    id: root

    property var reportInfo
    property int index
    property var model
    property var arr: []
    property bool isInitial: true
    property bool trigger: false
    readonly property bool isAssignedModeratorSelectedAccount: root.reportInfo.assignedModerator ? root.reportInfo.assignedAccount.userLevelIdentity.id === AccountManager.selectedAccountId : false
    readonly property bool assignedModerator: root.reportInfo.assignedAccount
    property bool displayAttachmentPanel: root.reportInfo.statusCount || root.reportInfo.mediaAttachmentCount
    readonly property bool largeScreen: width > Kirigami.Units.gridUnit * 25

    function isChecked(ruleId): bool {
        for (var i = 0; i < root.reportInfo.rules.length; i++) {
            if (ruleId === root.reportInfo.rules[i].id) {
                arr.push(ruleId)
                return true;
            }
        }
        return false;
    }

    function ammendOrDelete(inp): void {
        var index = arr.indexOf(inp);
        if (index !== -1) {
            arr.splice(index, 1);
        } else {
            arr.push(inp);
        }
    }

    title: i18n("Report #%1", root.reportInfo.reportId)

    actions: Kirigami.Action {
        icon.name: root.reportInfo.actionTaken ? "edit-delete-remove" : "checkmark"
        text: root.reportInfo.actionTaken ? i18nc("@action:intoolbar", "Mark as unresolved") : i18nc("@action:intoolbar", "Mark as resolved")
        onTriggered: {
            if (root.reportInfo.actionTaken) {
                root.model.unresolveReport(root.index)
                showPassiveNotification(i18nc("@info", "Report Unresolved"))
            } else {
                root.model.resolveReport(root.index)
                showPassiveNotification(i18nc("@info", "Report Resolved"))
            }
        }
    }

    RulesModel {
        id: rulesModel
        account: AccountManager.selectedAccount
    }

    AccountHeader {
        id: header

        identity: root.reportInfo.targetAccount.userLevelIdentity
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

        FormCard.FormTextDelegate {
            text: i18nc("@info Time at which the report was made", "Reported")
            description: root.reportInfo.createdAt.toLocaleString()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info The last time the reported account was active.", "Last Active")
            description: root.reportInfo.targetAccount.lastActive.toLocaleString()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info Time at which the reported account joined.", "Joined")
            description: root.reportInfo.targetAccount.joined.toLocaleString()
        }

        FormCard.FormDelegateSeparator {
            visible: root.reportInfo.filedAccount
        }

        FormCard.FormTextDelegate {
            visible: root.reportInfo.filedAccount
            text: i18nc("@label", "Reported By")
            description: root.reportInfo.filedAccount.userLevelIdentity.account

            leadingPadding: Kirigami.Units.largeSpacing
            leading: KirigamiComponents.Avatar {
                source: root.reportInfo.filedAccount.userLevelIdentity.avatarUrl
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 2
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            visible: true
            text: i18nc("@label", "Report Status")
            description: root.reportInfo.actionTaken ? i18nc("@info", "Resolved") : i18nc("@info", "Unresolved")
        }

        FormCard.FormDelegateSeparator {
            visible: root.reportInfo.actionTakenByAccount.userLevelIdentity.account
        }

        FormCard.FormTextDelegate {
            visible: root.reportInfo.actionTakenByAccount.userLevelIdentity.account
            text: i18nc("@label", "Action taken by")
            description: root.reportInfo.actionTakenByAccount.userLevelIdentity.account
            leadingPadding: Kirigami.Units.largeSpacing
            leading: KirigamiComponents.Avatar {
                source: root.reportInfo.actionTakenByAccount.userLevelIdentity.avatarUrl
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 2
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@label", "Assigned moderator")
            description: root.reportInfo.assignedModerator ? root.reportInfo.assignedAccount.userLevelIdentity.account : i18nc("@info", "No one")
            leadingPadding: Kirigami.Units.largeSpacing
            leading: KirigamiComponents.Avatar {
                source: root.reportInfo.assignedModerator ?  root.reportInfo.assignedAccount.userLevelIdentity.avatarUrl : ""
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 2
            }
            trailing: QQC2.Button {
                text: assignedModerator ? (isAssignedModeratorSelectedAccount ? i18nc("@action:button", "Unassign") : i18nc("@action:button", "Assign to me")) : i18nc("@action:button", "Assign to me")
                icon.name: "im-user"
                onClicked: assignedModerator ? (isAssignedModeratorSelectedAccount ? root.model.unassignReport(root.index) : root.model.assignReport(root.index)) : root.model.assignReport(root.index)
            }
        }

        FormCard.FormDelegateSeparator {
            visible: !root.reportInfo.targetAccount.isLocal
        }

        FormCard.FormTextDelegate {
            visible: !root.reportInfo.targetAccount.isLocal
            text: i18nc("@label", "Forwarded")
            description: root.reportInfo.forwarded ? i18nc("@info:The report is forwarded", "Yes") : i18nc("@info:The report is not forwarded", "No")
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Category")
    }

    FormCard.FormCard {
        QQC2.Label {
            text: i18n("The reason this account and/or content was reported will be cited in communication with the reported account")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormRadioDelegate {
            id: other
            text: i18n("Other")
            checked: root.reportInfo.category === "other"
            onCheckedChanged: if (checked && !isInitial) {
                arr = []
                root.model.updateReport(index, "other", arr)
                showPassiveNotification(i18n("Category changed to other"))
            } else {
                isInitial = false
            }
        }

        FormCard.FormDelegateSeparator { below: other ; above: spam }

        FormCard.FormRadioDelegate {
            id: spam
            checked: root.reportInfo.category === "spam"
            text: i18n("Spam")
            onCheckedChanged: if (checked && !isInitial) {
                arr = []
                root.model.updateReport(index, "spam", arr)
                showPassiveNotification(i18n("Category changed to spam"))
            } else {
                isInitial = false
            }
        }

        FormCard.FormDelegateSeparator { below: spam }

        FormCard.FormRadioDelegate {
            id: serverRules
            checked: root.reportInfo.category === "violation"
            text: i18n("Content violates one or more server rules")
            onCheckedChanged: if (checked && !isInitial) {
                arr = []
                root.model.updateReport(index, "violation", arr)
                showPassiveNotification(i18n("Category changed to rule violation"))
            } else {
                isInitial = false
            }
        }

        Repeater {
            id: rulesList

            model: serverRules.checked ? rulesModel : []

            delegate: ColumnLayout {
                id: ruleLayout

                required property int index
                required property string text
                required property string id

                spacing: 0
                FormCard.FormCheckDelegate {
                    id: ruleLabel
                    focusPolicy: Qt.NoFocus
                    hoverEnabled: false
                    Layout.leftMargin: Kirigami.Units.largeSpacing * 4
                    text: ruleLayout.text
                    checked: isChecked(ruleLayout.id)
                    Component.onCompleted: if (ruleLayout.index === rulesList.model.length -1) {
                        root.trigger = true
                    }
                    onCheckedChanged: if (root.trigger) {
                        ammendOrDelete(ruleLayout.id)
                        root.model.updateReport(ruleLayout.index, "violation", arr)
                    }
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "To provide more information, %1 wrote:", root.reportInfo.filedAccount.userLevelIdentity.account)
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            visible: true
            text: root.reportInfo.filedAccount.userLevelIdentity.account
            trailing: QQC2.Label {
                text: root.reportInfo.createdAt.toLocaleDateString()
            }
            description: root.reportInfo.comment ? root.reportInfo.comment : i18nc("@info:Account didn't provide any comment on the report ","N/A")
            leadingPadding: Kirigami.Units.largeSpacing
            leading: KirigamiComponents.Avatar {
                source: root.reportInfo.filedAccount.userLevelIdentity.avatarUrl
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 2
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Reported Content")
        visible: root.displayAttachmentPanel
    }

    FormCard.FormCard {
        visible: root.displayAttachmentPanel

        QQC2.Label {
            text: i18n("Offending content will be cited in communication with the reported account")
            wrapMode: Text.WordWrap
            Layout.margins: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        }

        FormCard.FormDelegateSeparator {}

        Repeater {
            id: statusList
            model: root.reportInfo.reportStatus

            delegate: ColumnLayout {
                required property var modelData
                spacing: 0
                Layout.fillWidth: true
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.gridUnit
                Layout.rightMargin: Kirigami.Units.gridUnit

                RowLayout {
                    Layout.preferredHeight: spoilerTextLabel.contentHeight + Kirigami.Units.gridUnit * 2
                    visible: modelData.spoilerText.length !== 0
                    Kirigami.Icon {
                        Layout.alignment: Qt.AlignVCenter
                        source: "data-warning"
                    }
                    QQC2.Label {
                        id: spoilerTextLabel
                        Layout.fillWidth: true
                        text: i18n("<b>Content Warning</b><br /> %1", modelData.spoilerText)
                        wrapMode: Text.Wrap
                        font: Config.defaultFont

                    }
                    QQC2.Button {
                        text: postContent.visible ? i18nc("@action:button", "Show Less") : i18nc("@action:button", "Show More")
                        onClicked: postContent.visible = !postContent.visible
                    }
                }

                PostContent {
                    id: postContent
                    content: modelData.content
                    expandedPost: false
                    secondary: false
                    visible: modelData.spoilerText.length === 0 || AccountManager.selectedAccount.preferences.extendSpoiler
                    shouldOpenInternalLinks: false
                }

                AttachmentGrid {
                    expandedPost: false
                    attachments: modelData.attachments
                    identity: modelData.authorIdentity
                    sensitive: true
                    secondary: false
                    inViewPort: true
                    viewportWidth: parent.width
                    visible: postContent.visible && modelData.attachments.length > 0
                }

                Item {
                    height: modelData.selected || Kirigami.Settings.tabletMode ? Kirigami.Units.mediumSpacing : Kirigami.Units.smallSpacing
                }

                RowLayout {
                    visible: true
                    QQC2.Label {
                        text: modelData.absoluteTime
                        elide: Text.ElideRight
                        color: Kirigami.Theme.disabledTextColor
                    }
                }
            }
        }
    }
}
