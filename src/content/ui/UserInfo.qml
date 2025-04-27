// SPDX-FileCopyrightText: 2022 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.labs.components 1 as KirigamiComponents
import org.kde.kirigamiaddons.statefulapp as StatefulApp

import org.kde.tokodon

QQC2.Pane {
    id: userInfo

    required property TokodonApplication application
    required property Kirigami.OverlayDrawer sidebar

    visible: AccountManager.selectedAccount
    padding: 0

    function openAccountPage() {
        // There's no way we can open the page if the account isn't working
        if (AccountManager.selectedAccountHasIssue) {
            return;
        }

        const accountId = AccountManager.selectedAccountId;
        if (!pageStack.currentItem.model || !pageStack.currentItem.model.accountId || accountId !== pageStack.currentItem.accountId) {
            Navigation.openAccount(accountId);
        }
    }

    contentItem: ColumnLayout {
        id: content

        spacing: 0

        Delegates.RoundedItemDelegate {
            id: currentAccountDelegate

            readonly property string name: {
                if (!AccountManager.selectedAccount) {
                    return '';
                }

                if (AccountManager.selectedAccount.identity.displayNameHtml.length !== 0) {
                    return AccountManager.selectedAccount.identity.displayNameHtml;
                }

                return AccountManager.selectedAccount.username;
            }

            text: name

            onClicked: {
                openAccountPage()
                if (userInfo.sidebar.modal) {
                    userInfo.sidebar.close();
                }
            }
            Layout.fillWidth: true

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.AbstractButton {
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.rightMargin: Kirigami.Units.smallSpacing

                    contentItem: KirigamiComponents.Avatar {
                        name: currentAccountDelegate.name
                        source: AccountManager.selectedAccount ? AccountManager.selectedAccount.identity.avatarUrl : ''
                    }

                    onClicked: openAccountPage()
                }

                Delegates.SubtitleContentItem {
                    subtitle: AccountManager.selectedAccount ? '@' + AccountManager.selectedAccount.username : ''
                    subtitleItem.textFormat: Text.PlainText
                    itemDelegate: currentAccountDelegate
                    Layout.fillWidth: true
                }

                QQC2.ToolButton {
                    icon.name: "system-switch-user"
                    onClicked: {
                        if (userInfo.sidebar.modal) {
                            userInfo.sidebar.close();
                        }

                        let dialog = Qt.createComponent("org.kde.tokodon", "AccountSwitchDialog").createObject(root.QQC2.Overlay.overlay, { application });
                        dialog.open();
                    }
                    text: i18n("Switch Account")
                    display: QQC2.AbstractButton.IconOnly
                    QQC2.ToolTip.text: i18n("Switch account")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Layout.minimumWidth: Layout.preferredWidth
                }
            }
        }
    }
}
