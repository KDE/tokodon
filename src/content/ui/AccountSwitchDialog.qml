// SPDX-FileCopyrightText: 2024 James Graham <james.h.graham@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kirigamiaddons.labs.components as KirigamiComponents
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.tokodon

Kirigami.Dialog {
    id: root

    required property TokodonApplication application

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

        footer: Delegates.RoundedItemDelegate {
            id: addDelegate
            width: parent.width
            highlighted: focus && !accountView.addAccount.pressed
            Component.onCompleted: accountView.addAccount = this
            icon {
                name: "list-add"
                width: Kirigami.Units.iconSizes.smallMedium
                height: Kirigami.Units.iconSizes.smallMedium
            }
            text: i18nc("@button: login to or register a new account.", "Add Account")
            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: parent
                subtitle: i18n("Log in or create a new account")
                labelItem.textFormat: Text.PlainText
                subtitleItem.textFormat: Text.PlainText
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
        Keys.onEnterPressed: accountView.currentItem.clicked()
        Keys.onReturnPressed: accountView.currentItem.clicked()

        onVisibleChanged: {
            for (let i = 0; i < accountView.count; i++) {
                if (model.data(model.index(i, 0), Qt.DisplayRole) === AccountManager.selectedAccountId) {
                    accountView.currentIndex = i;
                    break;
                }
            }
        }

        delegate: Delegates.RoundedItemDelegate {
            id: userDelegate

            required property int index
            required property string displayName
            required property string instance
            required property var account

            width: parent.width
            text: displayName

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

                Delegates.SubtitleContentItem {
                    itemDelegate: userDelegate
                    subtitle: userDelegate.instance
                    labelItem.textFormat: Text.PlainText
                    subtitleItem.textFormat: Text.PlainText
                }
            }

            onClicked: {
                if (AccountManager.selectedAccount !== userDelegate.account) {
                    AccountManager.selectedAccount = userDelegate.account;
                    accounts.currentIndex = userDelegate.index;
                }
                if (userInfo.sidebar.modal) {
                    userInfo.sidebar.close();
                }
                root.close();
            }
        }
    }
}
