// SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.kde.kirigami 2.15 as Kirigami

import org.kde.kmasto 1.0

Kirigami.Page {
    title: i18n("Accounts")

    leftPadding: pageSettingStack.wideMode ? Kirigami.Units.smallSpacing : 0
    topPadding: pageSettingStack.wideMode ? Kirigami.Units.smallSpacing : 0
    bottomPadding: pageSettingStack.wideMode ? Kirigami.Units.smallSpacing : 0
    rightPadding: pageSettingStack.wideMode ? Kirigami.Units.smallSpacing : 0

    actions.main: Kirigami.Action {
        text: i18n("Add an account")
        icon.name: "list-add-user"
        onTriggered: pageStack.layers.push("qrc:/content/ui/LoginPage.qml")
        visible: !pageSettingStack.wideMode
    }

    Connections {
        target: pageSettingStack
        function onWideModeChanged() {
            scroll.background.visible = pageSettingStack.wideMode
        }
    }

    Controls.ScrollView {
        id: scroll
        Component.onCompleted: background.visible = pageSettingStack.wideMode

        anchors.fill: parent

        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
        ListView {
            clip: true
            model: AccountManager
            delegate: Kirigami.SwipeListItem {
                leftPadding: 0
                rightPadding: 0
                Kirigami.BasicListItem {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    leading: Kirigami.Avatar {
                        source: model.account.identity.avatarUrl
                        name: model.display
                        implicitWidth: height
                        sourceSize.width: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                        sourceSize.height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                    }
                    label: model.display
                    subtitle: model.description
                    labelItem.textFormat: Text.PlainText

                    onClicked: {
                        if (AccountManager.selectedAccount !== model.account) {
                            AccountManager.selectedAccount = model.account;
                        }
                        pageStack.layers.pop()
                    }
                }
                actions: [
                    // TODO
                    //Kirigami.Action {
                    //    text: i18n("Edit this account")
                    //    iconName: "document-edit"
                    //    onTriggered: {
                    //        userEditSheet.connection = model.connection
                    //        userEditSheet.open()
                    //    }
                    //},
                    Kirigami.Action {
                        text: i18n("Logout")
                        iconName: "im-kick-user"
                        onTriggered: AccountManager.removeAccount(model.account)
                    }
                ]
            }
        }
    }

    footer: Column {
        height: visible ? implicitHeight : 0
        Kirigami.ActionToolBar {
            alignment: Qt.AlignRight
            visible: pageSettingStack.wideMode
            rightPadding: Kirigami.Units.smallSpacing
            width: parent.width
            flat: false
            actions: Kirigami.Action {
                text: i18n("Add an account")
                icon.name: "list-add-user"
                onTriggered: pageStack.layers.push("qrc:/content/ui/LoginPage.qml")
            }
        }
        Item {
            width: parent.width
            height: Kirigami.Units.smallSpacing
        }
    }
}
