// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kirigamiaddons.labs.components 1.0 as Components
import org.kde.kirigamiaddons.delegates 1.0 as Delegates

Kirigami.ScrollablePage {
    id: root

    property int index
    property var model
    property int id
    property string ip
    property int severity
    property string comment
    property date createdAt
    property date expiredAt

    title: root.ip

    property string displaySeverity: {
        if (root.severity === IpInfo.LimitSignUps) {
            return i18nc("@label", "Limit sign-ups");
        } else if (root.severity === IpInfo.BlockSignUps) {
            return i18nc("@label", "Block sign-ups");
        } else {
            return i18nc("@label", "Block access");
        }
    }


    actions.contextualActions: Kirigami.Action {
        icon.name: "edit-delete-remove"
        text: i18nc("@action:inmenu", "Remove IP Rule")
        onTriggered: {
            root.model.deleteIpBlock(root.index)
            showPassiveNotification(i18n("IP Rule Removed"))
            pageStack.layers.pop()
        }
    }

    Kirigami.PromptDialog {
        id: updateIpRuleDialog

        title: i18nc("@title:window", "Update IP Rule")

        property string calculatedseverity: signupLimit.checked ? "sign_up_requires_approval" : signupBlock.checked ? "sign_up_block" : "no_access"

        contentPadding: 0
        implicitWidth: Kirigami.Units.gridUnit * 20

        mainItem: ColumnLayout {
            spacing: 0

            MobileForm.FormTextDelegate {
                id: ip
                text: i18nc("@info IP address of the ip block", "IP")
                description: root.ip
            }

            MobileForm.FormDelegateSeparator {below: ip; above: expireAfter}

            MobileForm.FormComboBoxDelegate {
                id: expireAfter
                property var currentDate: new Date()
                text: i18nc("@info Time after which the rule will be lifted", "Expire After")
                textRole: "display"
                valueRole: "value"
                model: [
                    {
                        display: i18nc("@info Option to block out the IP for 1 day", "1 day"),
                        value: IpRulesToolModel.Oneday
                    },
                    {
                        display: i18nc("@info Option to block out the IP for 2 weeks", "2 weeks"),
                        value: IpRulesToolModel.Twoweeks
                    },
                    {
                        display: i18nc("@info Option to block out the IP for 1 month", "1 month"),
                        value: IpRulesToolModel.Onemonth
                    },
                    {
                        display: i18nc("@info Option to block out the IP for 6 months", "6 month"),
                        value: IpRulesToolModel.Sixmonths
                    },
                    {
                        display: i18nc("@info Option to block out the IP for 1 year", "1 year"),
                        value: IpRulesToolModel.OneYear
                    },
                    {
                        display: i18nc("@info Option to block out the IP for 3 years", "3 year"),
                        value: IpRulesToolModel.ThreeYears
                    },
                ]
                onCurrentIndexChanged: root.expiredAt = new Date(currentDate.getTime() + (model[currentIndex].value * 1000));
                Component.onCompleted: {expireAfter.currentIndex = expireAfter.indexOfValue(IpRulesToolModel.Oneday);
                }
            }
            MobileForm.FormDelegateSeparator {below: expireAfter; above: comment}
            MobileForm.FormTextFieldDelegate {
                id: comment
                label: i18nc("@info The comment attached with the ip rule", "Comment")
                text: root.comment
                placeholderText: i18n("Optional. Remember why you added this rule.")

            }
            MobileForm.FormDelegateSeparator {below: comment; above: rule}
            MobileForm.FormCardHeader {
                id: rule
                title: i18nc("@info The rule attached with the ip rule", "Rule *")
                subtitle: i18n("Choose what will happen with requests from this IP")
            }

            MobileForm.FormRadioDelegate {
                id: signupLimit
                text: i18n("Limit sign-ups")
                description: i18n("New sign-ups will require your approval")
                onToggled: root.severity = IpInfo.LimitSignUps
            }

            MobileForm.FormRadioDelegate {
                id: signupBlock
                text: i18n("Block sign-ups")
                description: i18n("New sign-ups will not be possible")
                onToggled: root.severity = IpInfo.BlockSignUps
            }

            MobileForm.FormRadioDelegate {
                id: accessBlock
                text: i18n("Block access")
                description: i18n("Block access to all resources")
                onToggled: root.severity = IpInfo.BlockAccess
            }

            MobileForm.FormDelegateSeparator {below: accessBlock}
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info Cancel button to close the dailog", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: updateIpRuleDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@info Button to update an IP rule", "Update IP rule")
                icon.name: "checkbox"
                onTriggered: {
                    root.comment = comment.text
                    root.model.updateIpBlock(root.index, root.ip, updateIpRuleDialog.calculatedseverity, comment.text, expireAfter.currentValue)
                    showPassiveNotification(i18n("IP rule updated"))
                    updateIpRuleDialog.close();
                }
            }
        ]
    }

    ColumnLayout {
        id: layout

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            contentItem: ColumnLayout {
                spacing: 0
                MobileForm.FormTextDelegate {
                    text: i18n("Blocked at")
                    description: root.createdAt.toLocaleDateString()
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextDelegate {
                    text: i18nc("@info Time after which the rule will be lifted", "Expires at")
                    description: root.expiredAt.toLocaleDateString()
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextDelegate {
                    text: i18nc("@info The comment attached with the ip rule", "Comment")
                    description: root.comment.length !== 0 ? root.comment : i18nc("@info No public comment provided", "None")
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextDelegate {
                    text: i18nc("@info The severity to be applied by this IP rule", "Severity")
                    description: root.displaySeverity
                }

                MobileForm.FormDelegateSeparator {}
            }
        }
    }

    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18nc("@action:button", "Update IP Rule")
                icon.name: 'cell_edit'
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    updateIpRuleDialog.open()
                }
            }
        }
    }
}
