// SPDX-FileCopyrightText: 2022 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

import org.kde.kmasto 1.0

QQC2.ToolBar {
    id: userInfo
    visible: AccountManager.selectedAccount

    padding: 0

    height: content.height

    property alias accountsListVisible: accounts.visible
    property var addAccount

    function openAccountPage() {
        const accountId = AccountManager.selectedAccountId;
        if (!pageStack.currentItem.model.accountId || accountId !== pageStack.currentItem.accountId) {
            const item = pageStack.push('qrc:/content/ui/AccountInfo.qml', {
                accountId: accountId,
            });
        }
    }

    ColumnLayout {
        id: content
        width: parent.width

        spacing: 0

        ListView {
            id: accounts

            model: AccountManager
            currentIndex: AccountManager.selectedIndex

            header: Kirigami.Separator {}

            footer: Kirigami.BasicListItem {
                width: parent.width
                highlighted: focus
                background: Rectangle {
                    id: background
                    color: addAccount.backgroundColor

                    Rectangle {
                        anchors.fill: parent
                        visible: !Kirigami.Settings.tabletMode && addAccount.hoverEnabled
                        color: addAccount.activeBackgroundColor
                        opacity: {
                            if ((addAccount.highlighted || addAccount.ListView.isCurrentItem) && !addAccount.pressed) {
                                return .6
                            } else if (addAccount.hovered && !addAccount.pressed) {
                                return .3
                            } else {
                                return 0
                            }
                        }
                    }
                }
                Component.onCompleted: userInfo.addAccount = this
                icon: "list-add"
                text: i18n("Add Account")
                enabled: AccountManager.hasAccounts && applicationWindow().pageStack.get(0).objectName !== 'loginPage' && applicationWindow().pageStack.get(0).objectName !== 'authorizationPage' && (applicationWindow().pageStack.layers.depth === 1 || applicationWindow().pageStack.layers.get(1).objectName !== 'loginPage' && applicationWindow().pageStack.layers.get(1).objectName !== 'authorizationPage')

                subtitle: i18n("Log in to an existing account")
                onClicked: {
                    pageStack.layers.push('qrc:/content/ui/LoginPage.qml');
                    userInfo.accountsListVisible = false
                    accounts.currentIndex = AccountManager.selectedIndex
                }
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

            width: parent.width
            Layout.preferredHeight: contentHeight
            delegate: Kirigami.BasicListItem {
                leftPadding: topPadding
                leading: Kirigami.Avatar {
                    source: model.account.identity.avatarUrl
                    name: model.display
                    implicitWidth: height
                    sourceSize.width: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                    sourceSize.height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                }
                width: parent.width
                //labelItem.textFormat: Text.PlainText
                subtitleItem.textFormat: Text.PlainText
                label: model.display
                subtitle: model.instance

                onClicked: {
                    if (AccountManager.selectedAccount !== model.account) {
                        AccountManager.selectedAccount = model.account;
                        accounts.currentIndex = index;
                    }
                    userInfo.accountsListVisible = false
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Item {
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Kirigami.Avatar {
                        anchors.fill: parent
                        anchors.margins: Kirigami.Units.smallSpacing
                        source: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.avatarUrl : ''
                        name: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.displayName : 'user'
                        actions.main: Kirigami.Action {
                            onTriggered: openAccountPage()
                        }
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    QQC2.Label {
                        id: displayNameLabel
                        text: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.displayNameHtml : ''

                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    QQC2.Label {
                        text: AccountManager.selectedAccount ? AccountManager.selectedAccount.instanceName : ''
                        font.pointSize: displayNameLabel.font.pointSize * 0.8
                        opacity: 0.7
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                TapHandler {
                    gesturePolicy: TapHandler.WithinBounds
                    onTapped: openAccountPage()
                }

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }
            QQC2.ToolButton {
                icon.name: "system-switch-user"
                onClicked: {
                    userInfo.accountsListVisible = !userInfo.accountsListVisible
                }
                text: i18n("Switch user")
                display: QQC2.AbstractButton.IconOnly
                Accessible.name: text
                QQC2.ToolTip {
                    text: parent.text
                }
                Layout.minimumWidth: Layout.preferredWidth
                Layout.alignment: Qt.AlignRight
            }
            QQC2.ToolButton {
                icon.name: "list-add"
                onClicked: ; //TODO
                text: i18n("Add") //TODO find better message
                display: QQC2.AbstractButton.IconOnly
                QQC2.ToolTip {
                    text: parent.text
                }
                Layout.minimumWidth: Layout.preferredWidth
                Layout.alignment: Qt.AlignRight
                visible: false
            }
            QQC2.ToolButton {
                icon.name: "lock"
                onClicked: pageStack.pushDialogLayer('qrc:/content/ui/ModerationTools/ModerationToolPage.qml', {}, { title: i18n("Moderation Tools") })
                text: i18nc("@action:button", "Open Moderation Tools")
                display: QQC2.AbstractButton.IconOnly
                QQC2.ToolTip.text: text
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                Layout.minimumWidth: Layout.preferredWidth
                Layout.alignment: Qt.AlignRight
                visible: AccountManager.selectedAccount && (AccountManager.selectedAccount.identity.permission & AdminAccountInfo.ManageUsers)
            }
            QQC2.ToolButton {
                icon.name: "settings-configure"
                onClicked: pageStack.pushDialogLayer('qrc:/content/ui/Settings/SettingsPage.qml', {}, { title: i18n("Configure") })
                text: i18n("Open settings")
                display: QQC2.AbstractButton.IconOnly
                Layout.minimumWidth: Layout.preferredWidth
                Layout.alignment: Qt.AlignRight
                QQC2.ToolTip {
                    text: parent.text
                }
            }
            Item {
                width: 1
            }
        }
    }
}
