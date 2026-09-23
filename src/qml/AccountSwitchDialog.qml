// SPDX-FileCopyrightText: 2024 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.tokodon

Kirigami.Dialog {
    id: root

    required property TokodonApplication application
    required property UserInfo userInfo

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    standardButtons: Kirigami.Dialog.NoButton
    width: Math.min(applicationWindow().width, Kirigami.Units.gridUnit * 24)
    title: i18nc("@title: dialog to switch between logged in accounts", "Switch Account")

    onVisibleChanged: if (visible) {
        accountView.forceActiveFocus()
    }

    contentItem: ListView {
        id: accountView
        property var addAccount

        implicitHeight: contentHeight

        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        currentIndex: AccountManager.selectedIndex

        footer: QQC2.ItemDelegate {
            id: addDelegate
            width: ListView.view.width
            highlighted: focus && !accountView.addAccount.pressed
            Component.onCompleted: accountView.addAccount = this
            icon {
                name: "list-add"
                width: Kirigami.Units.iconSizes.smallMedium
                height: Kirigami.Units.iconSizes.smallMedium
            }
            text: i18nc("@button: login to or register a new account.", "Add Account")
            contentItem: Kirigami.IconTitleSubtitle {
                title: addDelegate.text
                subtitle: i18n("Log in or create a new account")
                icon: icon.fromControlsIcon(addDelegate.icon)
                selected: addDelegate.highlighted || addDelegate.down
                font: addDelegate.font

                // TODO: Remove when we can depend on Kirigami 6.31
                Component.onCompleted: {
                    if (textFormat !== undefined) {
                        textFormat = Text.PlainText;
                    }
                }
            }

            action: Kirigami.Action {
                fromQAction: root.application.action('add_account')
            }

            onClicked: root.close()

            Keys.onUpPressed: {
                accountView.currentIndex = accountView.count - 1;
                accountView.forceActiveFocus();
            }
            Keys.onDownPressed: {
                accountView.currentIndex = 0;
                accountView.forceActiveFocus();
            }
        }
        clip: true
        model: AccountManager

        keyNavigationEnabled: false
        Keys.onDownPressed: {
            if (accountView.currentIndex === accountView.count - 1) {
                accountView.addAccount.forceActiveFocus();
                accountView.currentIndex = -1;
            } else {
                accountView.incrementCurrentIndex();
            }
        }
        Keys.onUpPressed: {
            if (accountView.currentIndex === 0) {
                accountView.addAccount.forceActiveFocus();
                accountView.currentIndex = -1;
            } else {
                accountView.decrementCurrentIndex();
            }
        }
        Keys.onEnterPressed: (accountView.currentItem as QQC2.AbstractButton).clicked()
        Keys.onReturnPressed: (accountView.currentItem as QQC2.AbstractButton).clicked()

        onVisibleChanged: {
            for (let i = 0; i < accountView.count; i++) {
                if (model.data(model.index(i, 0), Qt.DisplayRole) === AccountManager.selectedAccountId) {
                    accountView.currentIndex = i;
                    break;
                }
            }
        }

        delegate: QQC2.ItemDelegate {
            id: userDelegate

            required property int index
            required property string displayName
            required property string instance
            required property var account
            required property bool hasIssue

            width: ListView.view.width
            text: displayName
            highlighted: focus && !accountView.addAccount.pressed

            contentItem: RowLayout {
                KirigamiComponents.Avatar {
                    implicitWidth: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing
                    implicitHeight: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing
                    sourceSize {
                        width: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing
                        height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing
                    }
                    source: userDelegate.account.identity.avatarUrl
                    name: userDelegate.displayName
                }

                Kirigami.TitleSubtitle {
                    title: userDelegate.text
                    subtitle: userDelegate.instance
                    textFormat: Text.PlainText
                    selected: userDelegate.highlighted || userDelegate.down
                    font: userDelegate.font
                    Layout.fillWidth: true
                }

                QQC2.Control {
                    id: iconContainer

                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                    Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium

                    contentItem: Kirigami.Icon {
                        source: "data-warning"
                        visible: userDelegate.hasIssue

                        QQC2.ToolTip.text: i18nc("@info:tooltip", "This account has an issue and can't login, switch to it for more details.")
                        QQC2.ToolTip.visible: iconContainer.hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }
                }
            }

            onClicked: {
                if (AccountManager.selectedAccount !== userDelegate.account) {
                    AccountManager.selectedAccount = userDelegate.account;
                    accountView.currentIndex = userDelegate.index;
                }
                if (root.userInfo.sidebar.modal) {
                    root.userInfo.sidebar.close();
                }
                root.close();
            }
        }
    }
}
