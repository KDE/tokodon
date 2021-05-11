// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.ApplicationWindow {
    globalDrawer: Kirigami.GlobalDrawer {
        title: "Hello App"
        titleIcon: "applications-graphics"
        isMenu: true
    }

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
                        pageStack.clear();
                        pageStack.push(mainTimeline)
                    }
                }
            }
        }
    }

    Component {
        id: mainTimeline
        TimelinePage {
            model: TimelineModel {
                id: timelineModel
                accountManager: AccountManager
                name: "home"
            }
        }
    }
}
