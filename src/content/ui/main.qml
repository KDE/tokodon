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
                QQC2.TextField {
                    id: instanceUrl
                    Kirigami.FormData.label: i18n("Instance Url:")
                }
                QQC2.TextField {
                    id: username
                    Kirigami.FormData.label: i18n("Username:")
                }
            }
            footer: QQC2.ToolBar {
                RowLayout {
                    QQC2.Button {
                        text: i18n("Continue")
                        onClicked: pageStack.push(authorizationPage, {
                            account: AccountManager.createNewAccount(username.text, instanceUrl.text)
                        });
                    }
                }
            }
        }
    }

    Component {
        id: authorizationPage
        Kirigami.Page {
            required property var account

            ColumnLayout {
                anchors.centerIn: parent
                QQC2.TextField {
                    text: account.authorizeUrl
                }

                QQC2.Button {
                    text: "Open browser"
                    onClicked: Qt.openUrlExternally(account.authorizeUrl)
                }

                QQC2.TextField {
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
