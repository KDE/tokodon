// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kmasto 1.0

MastoPage {
    objectName: 'authorizationPage'
    property var account
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
            QQC2.Button {
                text: i18n("Open link")
                onClicked: Qt.openUrlExternally(account.authorizeUrl)
            }
            QQC2.Button {
                text: i18n("Copy link")
                onClicked: Clipboard.saveText(account.authorizeUrl)
            }
        }

        QQC2.Label {
            text: i18n("Enter token:")
        }

        QQC2.TextField {
            id: tokenField
            Layout.fillWidth: true
            onAccepted: continueButton.clicked()
        }

        QQC2.Button {
            id: continueButton
            text: i18n("Continue")
            onClicked: {
                if (!tokenField.text) {
                    applicationWindow().showPassiveNotification(i18n("Please insert the generated token."));
                    return;
                }
                account.setToken(tokenField.text);
                pageStack.layers.pop();
                if (pageStack.layers.depth > 1) {
                    pageStack.layers.pop();
                }
            }
        }
    }
}
