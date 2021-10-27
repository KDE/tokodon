// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import QtGraphicalEffects 1.12
import org.kde.kmasto 1.0

Kirigami.ApplicationWindow {
    id: appwindow

    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20
    maximumWidth: Kirigami.Units.gridUnit * 30
    maximumHeight: Kirigami.Units.gridUnit * 90
    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 30
    pageStack.globalToolBar.canContainHandles: true

    Connections {
        target: AccountManager
        function onAccountSelected() {
            pageStack.clear();
            pageStack.push(mainTimeline);
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        header: ColumnLayout {
            Repeater {
                model: AccountManager
                delegate: Kirigami.BasicListItem {
                    label: model.display
                    onClicked: {
                        AccountManager.selectedAccount = model.account;
                    }
                }
            }
        }
        actions: [
            Kirigami.Action {
                icon.name: "list-add"
                onTriggered: pageStack.push(loginPage);
                text: i18n("Add Account")
            }
        ]
    }

    footer: Kirigami.NavigationTabBar {
        visible: pageStack.layers.depth <= 1
        actions: [
            Kirigami.Action {
                iconName: "go-home-large"
                text: i18n("Home")
                checked: pageStack.currentItem.title === i18n("Home")
                onTriggered: {
                    pageStack.layers.clear();
                    pageStack.replace(mainTimeline);
                }
            },
            Kirigami.Action {
                iconName: "notifications"
                text: i18n("Notifications")
                checked: pageStack.currentItem.title === i18n("Home")
                onTriggered: appwindow.showPassiveNotification(i18n("Notifications support is not implemented yet"));
            },
            Kirigami.Action {
                iconName: "system-users"
                text: i18n("Local")
                checked: pageStack.currentItem.title === i18n("Local Timeline")
                onTriggered: {
                    pageStack.layers.clear();
                    pageStack.replace(mainTimeline, { name: "public" });
                }
            },
            Kirigami.Action {
                iconName: "kstars_xplanet"
                text: i18n("Global")
                checked: pageStack.currentItem.title === i18n("Global Timeline")
                onTriggered: {
                    pageStack.layers.clear();
                    pageStack.replace(mainTimeline, { name: "federated" });
                }
            }
        ]
    }
    //header: Kirigami.Settings.isMobile ? null : toolBar

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    Component.onCompleted: {
        if (AccountManager.hasAccounts) {
            pageStack.push(mainTimeline);
        } else {
            pageStack.push(loginPage);
        }
    }

    Component {
        id: loginPage
        MastoPage {
            Kirigami.FormLayout {
                anchors.centerIn: parent
                Kirigami.Heading {
                    Kirigami.FormData.isSection: true
                    text: i18n("Welcome to Tokodon")
                }
                QQC2.TextField {
                    id: instanceUrl
                    Kirigami.FormData.label: i18n("Instance Url:")
                }
                QQC2.TextField {
                    id: username
                    Kirigami.FormData.label: i18n("Username:")
                }
                QQC2.Button {
                    text: i18n("Continue")
                    onClicked: pageStack.push(authorizationPage, {
                        account: AccountManager.createNewAccount(username.text, instanceUrl.text)
                    });
                }
            }
        }
    }

    Component {
        id: authorizationPage
        MastoPage {
            required property var account
            Kirigami.FlexColumn {
                maximumWidth: Kirigami.Units.gridUnits * 30
                TextEdit {
                    color: Kirigami.Theme.textColor
                    textFormat: Text.RichText
                    readOnly: true
                    selectByMouse: true
                    text: i18n("To continue, please open the following link and authorize Tokodon: %1", "<br /><a href='" + account.authorizeUrl + "'>" + account.authorizeUrl + "</a>")
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(account.authorizeUrl)
                    Layout.fillWidth: true
                    TapHandler {
                        acceptedButtons: Qt.RightButton
                        onTapped: if (parent.hoveredLink.length > 0) {
                            menuLink.link = parent.hoveredLink;
                            menuLink.popup();
                        }
                    }
                    QQC2.Menu {
                        id: menuLink
                        property string link
                        QQC2.MenuItem {
                            text: i18n("Copy link")
                            onTriggered: Clipboard.saveText(menuLink.link)
                        }
                        QQC2.MenuItem {
                            text: i18n("Open link")
                            onTriggered: Qt.openUrlExternally(menuLink.link)
                        }
                    }
                }

                RowLayout {
                    QQC2.Button { text: i18n("Open link"); onClicked: Qt.openUrlExternally(account.authorizeUrl) }
                    QQC2.Button { text: i18n("Copy link"); onClicked: Clipboard.saveText(account.authorizeUrl) }
                }

                QQC2.Label {
                    text: i18n("Enter token:")
                }

                QQC2.TextField {
                    Layout.fillWidth: true
                    id: tokenField
                }

                QQC2.Button {
                    text: i18n("Continue")
                    onClicked: {
                        account.setToken(tokenField.text);
                    }
                }
            }
        }
    }

    Component {
        id: mainTimeline
        TimelinePage {
            property alias name: timelineModel.name
            model: TimelineModel {
                id: timelineModel
                accountManager: AccountManager
                name: "home"
            }
        }
    }
}
