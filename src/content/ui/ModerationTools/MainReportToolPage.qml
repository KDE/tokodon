// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import org.kde.tokodon.private
import Qt.labs.platform
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import Qt5Compat.GraphicalEffects
import "../StatusDelegate"

Kirigami.ScrollablePage {
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

    function isChecked(ruleId) {
        for (var i = 0; i < root.reportInfo.rules.length; i++) {
            if (ruleId === root.reportInfo.rules[i].id) {
                arr.push(ruleId)
                return true;
            }
        }
        return false;
    }

    function ammendOrDelete(inp) {
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
        text: root.reportInfo.actionTaken ? i18n("Mark as unresolved") : i18n("Mark as resolved")
        onTriggered: {
            if (root.reportInfo.actionTaken) {
                root.model.unresolveReport(root.index)
                showPassiveNotification(i18n("Report Unresolved"))
            } else {
                root.model.resolveReport(root.index)
                showPassiveNotification(i18n("Report Resolved"))
            }
        }
    }

    data: RulesModel {
        id: rulesModel
        account: AccountManager.selectedAccount
    }

    ColumnLayout {
        id: profileInfo

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            Rectangle {
                Layout.preferredHeight: Kirigami.Units.gridUnit * 9
                Layout.fillWidth: true
                clip: true
                color: Kirigami.Theme.backgroundColor
                Kirigami.Theme.colorSet: Kirigami.Theme.View

                Image {
                    id: bg
                    anchors.centerIn: parent
                    source: root.reportInfo.targetAccount.userLevelIdentity.backgroundUrl
                    fillMode: Image.PreserveAspectFit
                    visible: true
                }

                QQC2.Pane {
                    id: pane
                    visible: true
                    background: Item {
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
                        spacing: 0

                        RowLayout {
                            implicitHeight: Kirigami.Units.gridUnit * 5
                            KirigamiComponents.Avatar {
                                source: root.reportInfo.targetAccount.userLevelIdentity.avatarUrl
                            }

                            Column {
                                Layout.fillWidth: true
                                Kirigami.Heading {
                                    level: 5
                                    text: root.reportInfo.targetAccount.userLevelIdentity.username
                                    type: Kirigami.Heading.Primary
                                }
                                QQC2.Label {
                                    text: "@" + root.reportInfo.targetAccount.userLevelIdentity.account
                                }
                            }
                        }
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            RowLayout {
                visible: root.reportInfo.targetAccount.userLevelIdentity.bio
                spacing: 0
                QQC2.Pane {
                    contentItem: QQC2.Label {
                        text: i18nc("@info Bio label of account.", "Bio")
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
                    text: root.reportInfo.targetAccount.userLevelIdentity.bio
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
        }

        FormCard.FormGridContainer {
            id: container

            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            infoCards: [
                FormCard.FormGridContainer.InfoCard {
                    title: root.reportInfo.targetAccount.userLevelIdentity.statusesCount
                    subtitle: i18nc("@info Number of Posts", "Posts")
                },
                FormCard.FormGridContainer.InfoCard {
                    title: root.reportInfo.targetAccount.userLevelIdentity.followersCount
                    subtitle: i18nc("@info Number of followers.", "Followers")
                },
                FormCard.FormGridContainer.InfoCard {
                    title: root.reportInfo.targetAccount.userLevelIdentity.followingCount
                    subtitle: i18nc("@info Row Number of accounts followed by the account", "Following")
                },
                FormCard.FormGridContainer.InfoCard {
                    title: root.reportInfo.targetAccount.joined.toLocaleString()
                    subtitle: i18nc("@info Time at which the reported account joined.", "Joined")
                },
                FormCard.FormGridContainer.InfoCard {
                    title: root.reportInfo.targetAccount.lastActive.toLocaleString()
                    subtitle: i18nc("@info The last time the reported account was active.", "Last Active")
                },
                FormCard.FormGridContainer.InfoCard {
                    title: root.reportInfo.createdAt.toLocaleString()
                    subtitle: i18nc("@info The current login status of the account.", "Reported")
                }
            ]
        }

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: true

            FormCard.FormTextDelegate {
                visible: true
                text: i18nc("@info Time at which the report was made", "Reported")
                description: root.reportInfo.createdAt.toLocaleString()
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                visible: root.reportInfo.filedAccount
                text: i18n("Reported By")
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
                text: i18n("Report Status")
                description: root.reportInfo.actionTaken ? i18n("Resolved") : i18n("Unresolved")
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                visible: root.reportInfo.actionTakenByAccount.userLevelIdentity.account
                text: i18n("Action taken by")
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
                text: i18n("Assigned moderator")
                description: root.reportInfo.assignedModerator ? root.reportInfo.assignedAccount.userLevelIdentity.account : i18n("No one")
                leadingPadding: Kirigami.Units.largeSpacing
                leading: KirigamiComponents.Avatar {
                    source: root.reportInfo.assignedModerator ?  root.reportInfo.assignedAccount.userLevelIdentity.avatarUrl : ""
                    implicitHeight: Kirigami.Units.gridUnit * 2
                    implicitWidth: Kirigami.Units.gridUnit * 2
                }
                trailing: QQC2.Button {
                    text: assignedModerator ? (isAssignedModeratorSelectedAccount ? i18n("Unassign") : i18n("Assign to me")) : i18n("Assign to me")
                    icon.name: "im-user"
                    onClicked: assignedModerator ? (isAssignedModeratorSelectedAccount ? root.model.unassignReport(root.index) : root.model.assignReport(root.index)) : root.model.assignReport(root.index)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                visible: !root.reportInfo.targetAccount.isLocal
                text: i18n("Forwarded")
                description: root.reportInfo.forwarded ? i18nc("@info:The report is forwarded", "Yes") : i18nc("@info:The report is not forwarded", "No")
            }
        }

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            FormCard.FormHeader {
                title: i18n("Category")
            }

            QQC2.Label {
                text: i18n("The reason this account and/or content was reported will be cited in communication with the reported account")
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                Layout.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
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

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            FormCard.FormHeader {
                title: i18n("To provide more information, %1 wrote:", root.reportInfo.filedAccount.userLevelIdentity.account)
            }

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

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: root.displayAttachmentPanel

            FormCard.FormHeader {
                title: i18n("Reported Content")
            }

            QQC2.Label {
                text: i18n("Offending content will be cited in communication with the reported account")
                Layout.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
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
                            text: postContent.visible ? i18n("Show Less") : i18n("Show More")
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
                        visible: postContent.visible && modelData.attachments.length > 0
                    }

                    Item {
                        height: modelData.selected || Kirigami.Settings.tabletMode ? Kirigami.Units.mediumSpacing : Kirigami.Units.smallSpacing
                    }

                    RowLayout {
                        visible:true
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
}