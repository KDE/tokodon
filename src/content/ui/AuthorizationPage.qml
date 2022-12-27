// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

MastoPage {
    objectName: 'authorizationPage'
    property var account
    title: i18n("Authorization")

    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        width: parent.width
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Authorize Tokodon to act on your behalf")
                }

                MobileForm.AbstractFormDelegate {
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
                                text: i18n("Copy link")
                                onTriggered: Clipboard.saveText(menuLink.link)
                            }
                            QQC2.MenuItem {
                                text: i18n("Open link")
                                onTriggered: Qt.openUrlExternally(menuLink.link)
                            }
                        }
                    }
                }

                MobileForm.FormDelegateSeparator { above: openLink }

                MobileForm.FormButtonDelegate {
                    id: openLink
                    text: i18n("Open link")
                    onClicked: Qt.openUrlExternally(account.authorizeUrl)
                }

                MobileForm.FormDelegateSeparator { above: openLink; below: copyLink }

                MobileForm.FormButtonDelegate {
                    id: copyLink
                    text: i18n("Copy link")
                    onClicked: Clipboard.saveText(account.authorizeUrl)
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormTextFieldDelegate {
                    id: tokenField
                    label: i18n("Enter token:")
                    onAccepted: continueButton.clicked()
                }

                MobileForm.FormDelegateSeparator { below: continueButton; above: tokenField }

                MobileForm.FormButtonDelegate {
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
    }
}
