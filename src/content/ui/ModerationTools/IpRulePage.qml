// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.labs.components 1 as Components
import org.kde.kirigamiaddons.delegates 1 as Delegates

Kirigami.ScrollablePage {
    id: root

    actions: Kirigami.Action {
        text: i18nc("@action:button", "Create Rule")
        icon.name: 'list-add'
        onTriggered: newIpRuleDialog.open()
    }

    Kirigami.PromptDialog {
        id: newIpRuleDialog

        title: i18n("New IP Rule")

        property string calculatedSeverity: signupLimit.checked ? "sign_up_requires_approval" : signupBlock.checked ? "sign_up_block" : "no_access"

        contentPadding: 0
        implicitWidth: Kirigami.Units.gridUnit * 20

        mainItem: ColumnLayout {
            spacing: 0

            FormCard.FormTextFieldDelegate {
                id: ip
                label: i18nc("@info:Enter the domain address of the domain block", "IP*")
                placeholderText: "192.0.2.0/24"
            }
            FormCard.FormDelegateSeparator {above: expireAfter; below: ip}
            FormCard.FormComboBoxDelegate {
                id: expireAfter
                text: i18nc("@info:Time after which the rule will be lifted", "Expire After")
                textRole: "display"
                valueRole: "value"
                model: [
                    {
                        display: i18nc("@info:Option to block out the IP for 1 day", "1 day"),
                        value: IpRulesToolModel.Oneday
                    },
                    {
                        display: i18nc("@info:Option to block out the IP for 2 weeks", "2 weeks"),
                        value: IpRulesToolModel.Twoweeks
                    },
                    {
                        display: i18nc("@info:Option to block out the IP for 1 month", "1 month"),
                        value: IpRulesToolModel.Onemonth
                    },
                    {
                        display: i18nc("@info:Option to block out the IP for 6 months", "6 month"),
                        value: IpRulesToolModel.Sixmonths
                    },
                    {
                        display: i18nc("@info:Option to block out the IP for 1 year", "1 year"),
                        value: IpRulesToolModel.OneYear
                    },
                    {
                        display: i18nc("@info:Option to block out the IP for 3 years", "3 year"),
                        value: IpRulesToolModel.ThreeYears
                    },
                ]

                Component.onCompleted: {expireAfter.currentIndex = expireAfter.indexOfValue(IpRulesToolModel.Oneday);
                }
            }
            FormCard.FormDelegateSeparator {above: comment; below: expireAfter}
            FormCard.FormTextFieldDelegate {
                id: comment
                label: i18nc("@info: The comment attached with the ip rule", "Comment")
                placeholderText: i18n("Optional. Remember why you added this rule.")

            }
            FormCard.FormDelegateSeparator { below: comment }

            FormCard.FormHeader {
                id: rule
                title: i18nc("@info:The rule attached with the ip rule", "Rule *")
            }

            QQC2.Label {
                text: i18n("Choose what will happen with requests from this IP")
                Layout.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            }

            FormCard.FormRadioDelegate {
                id: signupLimit
                text: i18n("Limit sign-ups")
                description: i18n("New sign-ups will require your approval")
                checked: true
            }

            FormCard.FormRadioDelegate {
                id: signupBlock
                text: i18n("Block sign-ups")
                description: i18n("New sign-ups will not be possible")
            }

            FormCard.FormRadioDelegate {
                id: accessBlock
                text: i18n("Block access")
                description: i18n("Block access to all resources")
            }

            FormCard.FormDelegateSeparator { below: accessBlock }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info:Cancel button to close the dailog", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: newIpRuleDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@info:Button to create a IP rule", "Create IP rule")
                icon.name: "checkbox"
                onTriggered: {
                    ipRuleView.model.newIpBlock(ip.text, expireAfter.currentValue, comment.text, newIpRuleDialog.calculatedSeverity)
                    showPassiveNotification(i18n("New IP rule added"))
                    newIpRuleDialog.close();
                }
            }
        ]
    }

    ListView {
        id: ipRuleView

        model: IpRulesToolModel {}
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property int id
            required property string ip
            required property int severity
            required property string comment
            required property date createdAt
            required property date expiredAt

            property string displaySeverity: {
                if (delegate.severity === IpInfo.LimitSignUps) {
                    return i18nc("@label", "Limit sign-ups");
                } else if (delegate.severity === IpInfo.BlockSignUps) {
                    return i18nc("@label", "Block sign-ups");
                } else {
                    return i18nc("@label", "Block access");
                }
            }

            width: ListView.view.width

            onClicked: root.QQC2.ApplicationWindow.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "MainIpRulePage"),
                    {
                        index: delegate.index,
                        model: ipRuleView.model,
                        id: delegate.id,
                        ip: delegate.ip,
                        severity: delegate.severity,
                        comment: delegate.comment,
                        createdAt: delegate.createdAt,
                        expiredAt: delegate.expiredAt
                    })

            text: delegate.ip

            contentItem: RowLayout {
                Delegates.SubtitleContentItem {
                    itemDelegate: delegate
                    bold: true
                    subtitle: delegate.displaySeverity
                }

                Item {
                    Layout.fillWidth: true
                }

                Kirigami.Heading {
                    level: 3
                    text: delegate.createdAt.toLocaleDateString()
                    type: Kirigami.Heading.Type.Secondary
                    Layout.alignment: Qt.AlignRight
                }
            }
        }

        QQC2.ProgressBar {
            visible: ipRuleView.model.loading && ipRuleView.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No IP rules found")
            visible: ipRuleView.count === 0 && !ipRuleView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
