// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import QtGraphicalEffects 1.12
import org.kde.kmasto 1.0

Kirigami.ApplicationWindow {
    id: appwindow

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

    property QQC2.ToolBar toolBar: QQC2.ToolBar {
        id: toolbarRoot
        property int iconSize: Kirigami.Units.gridUnit * 2
        background: Rectangle {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow {
                color: Qt.rgba(0.0, 0.0, 0.0, 0.33)
                radius: 6
                samples: 8
            }
        }
        RowLayout {
            anchors.fill: parent
            spacing: 0
            Repeater {
                property list<QtObject> actionList: [
                    Kirigami.Action {
                        text: i18n("Home")
                        icon.name: "go-home-large"
                        enabled: pageStack.layers.depth > 1 || !isCurrentPage
                        property bool isCurrentPage: pageStack.currentItem.title === i18n("Home")
                        onTriggered: {
                            pageStack.layers.clear();
                            pageStack.replace(mainTimeline);
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Notifications")
                        icon.name: "notifications"
                        onTriggered: appwindow.showPassiveNotification(i18n("Notifications support is not implemented yet"));
                        property bool isCurrentPage: false
                    },
                    Kirigami.Action {
                        text: i18n("Local")
                        icon.name: "system-users"
                        enabled: pageStack.layers.depth > 1 || !isCurrentPage
                        property bool isCurrentPage: pageStack.currentItem.title === i18n("Local Timeline")
                        onTriggered: {
                            pageStack.layers.clear();
                            pageStack.replace(mainTimeline, { name: "public" });
                        }
                    },
                    Kirigami.Action {
                        text: i18n("Global")
                        icon.name: "kstars_xplanet"
                        property bool isCurrentPage: pageStack.currentItem.title === i18n("Global Timeline")
                        enabled: pageStack.layers.depth > 1 || !isCurrentPage
                        onTriggered: {
                            pageStack.layers.clear();
                            pageStack.replace(mainTimeline, { name: "federated" });
                        }
                    }
                ]
                model: actionList

                Rectangle {
                    Layout.minimumWidth: parent.width / 5
                    Layout.maximumWidth: parent.width / 5
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                    Layout.alignment: Qt.AlignCenter

                    Kirigami.Theme.colorSet: Kirigami.Theme.Window
                    Kirigami.Theme.inherit: false

                    color: mouseArea.pressed ? Qt.darker(Kirigami.Theme.backgroundColor, 1.1) : 
                           mouseArea.containsMouse ? Qt.darker(Kirigami.Theme.backgroundColor, 1.03) : Kirigami.Theme.backgroundColor

                    Behavior on color {
                        ColorAnimation { 
                            duration: 100 
                            easing.type: Easing.InOutQuad
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        hoverEnabled: true
                        anchors.fill: parent
                        onClicked: if (modelData.enabled) {
                            modelData.trigger();
                        }
                        onPressed: {
                            widthAnim.to = toolbarRoot.shrinkIconSize;
                            heightAnim.to = toolbarRoot.shrinkIconSize;
                            widthAnim.restart();
                            heightAnim.restart();
                        }
                        onReleased: {
                            if (!widthAnim.running) {
                                widthAnim.to = toolbarRoot.iconSize;
                                widthAnim.restart();
                            }
                            if (!heightAnim.running) {
                                heightAnim.to = toolbarRoot.iconSize;
                                heightAnim.restart();
                            }
                        }
                    }
                    
                    ColumnLayout {
                        id: itemColumn
                        anchors.fill: parent
                        spacing: Kirigami.Units.smallSpacing
        
                        Kirigami.Icon {
                            color: modelData.isCurrentPage ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                            source: model.icon.name
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                            Layout.preferredHeight: toolbarRoot.iconSize
                            Layout.preferredWidth: toolbarRoot.iconSize

                            ColorAnimation on color {
                                easing.type: Easing.Linear
                            }
                            NumberAnimation on Layout.preferredWidth {
                                id: widthAnim
                                easing.type: Easing.Linear
                                duration: 130
                                onFinished: {
                                    if (widthAnim.to !== toolbarRoot.iconSize && !mouseArea.pressed) {
                                        widthAnim.to = toolbarRoot.iconSize;
                                        widthAnim.start();
                                    }
                                }
                            }
                            NumberAnimation on Layout.preferredHeight {
                                id: heightAnim
                                easing.type: Easing.Linear
                                duration: 130
                                onFinished: {
                                    if (heightAnim.to !== toolbarRoot.iconSize && !mouseArea.pressed) {
                                        heightAnim.to = toolbarRoot.iconSize;
                                        heightAnim.start();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    footer: toolBar
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
