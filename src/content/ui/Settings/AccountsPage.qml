// SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.kde.kirigami 2.15 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    title: i18n("Accounts")

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    ColumnLayout {
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                MobileForm.FormCardHeader {
                    title: i18n("General")
                }

                MobileForm.FormSwitchDelegate {
                    id: showStats
                    text: i18n("Show detailed statistics about posts.")
                    checked: Config.showPostStats
                    enabled: !Config.isShowPostStatsImmutable
                    onToggled: {
                        Config.showPostStats = checked
                        Config.save()
                    }
                }

                MobileForm.FormDelegateSeparator { below: showStats; above: showLinkPreview }

                MobileForm.FormSwitchDelegate {
                    id: showLinkPreview
                    text: i18n("Show link preview.")
                    checked: Config.showLinkPreview
                    enabled: !Config.isShowLinkPreviewImmutable
                    onToggled: {
                        Config.showLinkPreview = checked
                        Config.save()
                    }
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                MobileForm.FormCardHeader {
                    title: i18n("Accounts")
                }

                Repeater {
                    model: AccountManager
                    delegate: MobileForm.FormButtonDelegate {
                        //onClicked: pageSettingStack.pushDialogLayer("qrc:/AccountEditorPage.qml", {
                        //    connection: model.connection
                        //}, {
                        //    title: i18n("Account editor")
                        //})

                        contentItem: RowLayout {
                            Kirigami.Avatar {
                                source: model.account.identity.avatarUrl
                                name: model.display
                                Layout.rightMargin: Kirigami.Units.largeSpacing
                                implicitWidth: Kirigami.Units.iconSizes.medium
                                implicitHeight: Kirigami.Units.iconSizes.medium
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: Kirigami.Units.smallSpacing

                                QQC2.Label {
                                    Layout.fillWidth: true
                                    text: model.display
                                    textFormat: Text.PlainText
                                    elide: Text.ElideRight
                                    wrapMode: Text.Wrap
                                    maximumLineCount: 2
                                    color: Kirigami.Theme.textColor
                                }

                                QQC2.Label {
                                    Layout.fillWidth: true
                                    text: `${model.description} (${model.account.instanceName})`
                                    color: Kirigami.Theme.disabledTextColor
                                    font: Kirigami.Theme.smallFont
                                    elide: Text.ElideRight
                                }
                            }

                            QQC2.ToolButton {
                                text: i18n("Logout")
                                icon.name: "im-kick-user"
                                onClicked: {
                                    AccountManager.removeAccount(model.account)
                                    if (!AccountManager.hasAccount) {
                                        settingsPage.closeDialog()
                                    }
                                }
                            }

                            MobileForm.FormArrow {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                direction: MobileForm.FormArrow.Right
                            }
                        }
                    }
                }

                MobileForm.FormDelegateSeparator { below: addAccountDelegate }

                MobileForm.FormButtonDelegate {
                    id: addAccountDelegate
                    text: i18n("Add Account")
                    icon.name: "list-add"
                    onClicked: pageStack.layers.push("qrc:/content/ui/LoginPage.qml")
                }
            }
        }

        SonnetConfigPage {}
    }
}
