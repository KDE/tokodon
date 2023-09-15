// SPDX-FileCopyrightText: 2022 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.labs.components 1 as KirigamiComponents

import org.kde.tokodon

QQC2.Pane {
    id: userInfo

    property alias accountsListVisible: accounts.visible
    property var addAccount

    visible: AccountManager.selectedAccount
    padding: 0

    function openAccountPage() {
        const accountId = AccountManager.selectedAccountId;
        if (!pageStack.currentItem.model.accountId || accountId !== pageStack.currentItem.accountId) {
            const item = pageStack.push(Qt.createComponent("org.kde.tokodon", "AccountInfo"), {
                accountId: accountId,
            });
        }
    }

    contentItem: ColumnLayout {
        id: content

        spacing: 0

        Delegates.RoundedItemDelegate {
            id: currentAccountDelegate

            text: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.displayNameHtml : ''

            onClicked: openAccountPage()
            Layout.fillWidth: true

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.AbstractButton {
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.rightMargin: Kirigami.Units.smallSpacing

                    contentItem: KirigamiComponents.Avatar {
                        name: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.displayName : 'User'
                        source: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.avatarUrl : ''
                    }

                    onClicked: openAccountPage()
                }

                Delegates.SubtitleContentItem {
                    subtitle: AccountManager.selectedAccount ? AccountManager.selectedAccount.instanceName : ''
                    subtitleItem.textFormat: Text.PlainText
                    itemDelegate: currentAccountDelegate
                    Layout.fillWidth: true
                }

                QQC2.ToolButton {
                    icon.name: "system-switch-user"
                    onClicked: {
                        userInfo.accountsListVisible = !userInfo.accountsListVisible
                    }
                    text: i18n("Switch user")
                    display: QQC2.AbstractButton.IconOnly
                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Layout.minimumWidth: Layout.preferredWidth
                }
            }
        }

        ListView {
            id: accounts

            model: AccountManager
            currentIndex: AccountManager.selectedIndex

            header: Kirigami.Separator {}

            footer: Delegates.RoundedItemDelegate {
                id: addAccountDelegaze

                width: parent.width
                highlighted: focus
                icon {
                    name: "list-add"
                    width: Kirigami.Units.iconSizes.medium
                    height: Kirigami.Units.iconSizes.medium
                }
                text: i18n("Add Account")
                enabled: AccountManager.hasAccounts && applicationWindow().pageStack.depth > 0 && applicationWindow().pageStack.get(0).objectName !== 'loginPage' && applicationWindow().pageStack.get(0).objectName !== 'authorizationPage' && (applicationWindow().pageStack.layers.depth === 1 || applicationWindow().pageStack.layers.get(1).objectName !== 'loginPage' && applicationWindow().pageStack.layers.get(1).objectName !== 'authorizationPage')

                contentItem: Delegates.SubtitleContentItem {
                    itemDelegate: addAccountDelegaze
                    subtitle: i18n("Log in to an existing account")
                }

                onClicked: {
                    pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "LoginPage"));
                    userInfo.accountsListVisible = false
                    accounts.currentIndex = AccountManager.selectedIndex
                }

                Component.onCompleted: userInfo.addAccount = this
                Keys.onUpPressed: {
                    accounts.currentIndex = accounts.count - 1
                    accounts.forceActiveFocus()
                }
                Keys.onDownPressed: {
                    accounts.currentIndex = 0
                    accounts.forceActiveFocus()
                }
            }

            visible: false
            onVisibleChanged: if (visible) focus = true
            clip: true

            keyNavigationEnabled: false
            Keys.onDownPressed: {
                if (accounts.currentIndex === accounts.count - 1) {
                    addAccount.forceActiveFocus()
                    accounts.currentIndex = -1
                } else {
                    accounts.incrementCurrentIndex()
                }
            }
            Keys.onUpPressed: {
                if (accounts.currentIndex === 0) {
                    addAccount.forceActiveFocus()
                    accounts.currentIndex = -1
                } else {
                    accounts.decrementCurrentIndex()
                }
            }

            Keys.onReleased: if (event.key == Qt.Key_Escape) {
                userInfo.accountsListVisible = false
            }

            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight

            delegate: Delegates.RoundedItemDelegate {
                id: accountDelegate

                required property int index
                required property string displayName
                required property string instance
                required property var account

                text: displayName

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    KirigamiComponents.Avatar {
                        source: accountDelegate.account.identity.avatarUrl
                        name: accountDelegate.displayName
                        Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                        Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                        Layout.leftMargin: Kirigami.Units.smallSpacing
                        Layout.rightMargin: Kirigami.Units.smallSpacing
                    }

                    Delegates.SubtitleContentItem {
                        itemDelegate: accountDelegate
                        subtitleItem.textFormat: Text.PlainText
                        subtitle: accountDelegate.instance
                        Layout.fillWidth: true
                    }
                }

                onClicked: {
                    if (AccountManager.selectedAccount !== accountDelegate.account) {
                        AccountManager.selectedAccount = accountDelegate.account;
                        accounts.currentIndex = accountDelegate.index;
                    }
                    userInfo.accountsListVisible = false
                }
            }
        }

    }
}
