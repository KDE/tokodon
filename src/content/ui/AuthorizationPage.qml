// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon
import org.kde.kirigamiaddons.formcard 1 as FormCard

MastoPage {
    objectName: 'authorizationPage'
    property var account
    title: i18n("Authorization")

    leftPadding: 0
    rightPadding: 0
    topPadding: 0

    ColumnLayout {
        width: parent.width

        FormCard.FormHeader {
            title: i18n("Authorize Tokodon to act on your behalf")
        }

        FormCard.FormCard {
            FormCard.AbstractFormDelegate {
                background: Item {}
                Layout.fillWidth: true

                contentItem: TextEdit {
                    color: Kirigami.Theme.textColor
                    textFormat: Text.RichText
                    readOnly: true
                    selectByMouse: true
                    text: i18n("To continue, please open the following link and authorize Tokodon: %1", "<br /><a href='" + account.authorizeUrl + "'>" + account.authorizeUrl + "</a>")
                    wrapMode: Text.Wrap
                    onLinkActivated: Qt.openUrlExternally(account.authorizeUrl)
                    Layout.fillWidth: true
                    TapHandler {
                        acceptedButtons: Qt.RightButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onTapped: if (parent.hoveredLink.length > 0) {
                            menuLink.link = parent.hoveredLink;
                            menuLink.popup();
                        }
                    }
                    QQC2.Menu {
                        id: menuLink
                        property string link
                        QQC2.MenuItem {
                            text: i18n("Open Link")
                            onTriggered: Qt.openUrlExternally(menuLink.link)
                        }

                        QQC2.MenuItem {
                            text: i18n("Copy Link")
                            onTriggered: {
                                Clipboard.saveText(menuLink.link)
                                applicationWindow().showPassiveNotification(i18n("Link copied."));
                            }
                        }
                    }
                }
            }

            FormCard.FormDelegateSeparator { above: openLink }

            FormCard.FormButtonDelegate {
                id: openLink
                text: i18n("Open Link")
                icon.name: "document-open"
                onClicked: Qt.openUrlExternally(account.authorizeUrl)
            }

            FormCard.FormDelegateSeparator { above: openLink; below: copyLink }

            FormCard.FormButtonDelegate {
                id: copyLink
                text: i18n("Copy Link")
                icon.name: "edit-copy"
                onClicked: {
                    Clipboard.saveText(account.authorizeUrl)
                    applicationWindow().showPassiveNotification(i18n("Link copied."));
                }
            }
        }
    }

    data: Connections {
        target: Controller

        function onReceivedAuthCode(authCode) {
            account.setToken(authCode);
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                name: "home"
            });
        }
    }
}
