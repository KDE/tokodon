// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kmasto 1.0
import Qt.labs.platform 1.1
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

Kirigami.ScrollablePage {
    property var account

    property Identity identity: Identity {
        id: newIdentity
    }

    title: i18n('Profile Editor')
    leftPadding: 0
    rightPadding: 0

    Component {
        id: openFileDialog
        FileDialog {
            signal chosen(string path)
            title: i18n("Please choose a file")
            folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
            onAccepted: chosen(file)
        }
    }

    ColumnLayout {
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                Rectangle {
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 9
                    Layout.fillWidth: true
                    clip: true
                    color: Kirigami.Theme.backgroundColor
                    Kirigami.Theme.colorSet: Kirigami.Theme.View

                    Image {
                        anchors.centerIn: parent
                        source: account.identity.backgroundUrl
                        fillMode: Image.PreserveAspectFit
                        visible: account.identity.backgroundUrl
                    }

                    QQC2.Pane {
                        background: Item {}
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        contentItem: RowLayout {
                            implicitHeight: Kirigami.Units.gridUnit * 5
                            Kirigami.Avatar {
                                source: account.identity.avatarUrl
                            }

                            Column {
                                Layout.fillWidth: true
                                Kirigami.Heading {
                                    level: 5
                                    text: account.identity.displayName
                                    type: Kirigami.Heading.Primary
                                }
                                QQC2.Label {
                                    text: '@' + account.username + '@' + account.instanceUri
                                }
                            }
                        }
                    }
                }

                MobileForm.FormTextFieldDelegate {
                    label: i18n("Display Name")
                    text: account.identity.displayName
                    onTextChanged: newIdentity.displayName = text
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.AbstractFormDelegate {
                    background: Item {}
                    Layout.fillWidth: true

                    contentItem: ColumnLayout {
                        QQC2.Label {
                            text: i18n("Bio")
                            Layout.fillWidth: true
                        }
                        QQC2.TextArea {
                            id: bioField
                            Layout.fillWidth: true
                            text: account.identity.bio
                            textFormat: TextEdit.RichText
                        }
                    }
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.AbstractFormDelegate {
                    background: Item {}
                    Layout.fillWidth: true
                    contentItem: ColumnLayout {
                        QQC2.Label {
                            text: i18n("Header")
                            Layout.fillWidth: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            QQC2.RoundButton {
                                id: headerUpload
                                icon.name: 'download'
                                property var fileDialog: null;
                                Layout.alignment: Qt.AlignHCenter
                                onClicked: {
                                    if (fileDialog !== null) {
                                        return;
                                    }

                                    fileDialog = openFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

                                    fileDialog.chosen.connect(function(receivedSource) {
                                        headerUpload.fileDialog = null;
                                        if (!receivedSource) {
                                            return;
                                        }
                                        account.identity.backgroundUrl = receivedSource;
                                    });
                                    fileDialog.onRejected.connect(function() {
                                        headerUpload.fileDialog = null;
                                    });
                                    fileDialog.open();
                                }
                            }
                            QQC2.Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: i18n("PNG, GIF or JPG. At most 2 MB. Will be downscaled to 1500x500px")
                                wrapMode: Text.WordWrap
                                color: Kirigami.Theme.disabledTextColor
                            }
                        }

                        Kirigami.LinkButton {
                            text: i18n("Delete")
                            color: Kirigami.Theme.negativeTextColor
                            Layout.bottomMargin: Kirigami.Units.largeSpacing
                        }
                    }
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.AbstractFormDelegate {
                    background: Item {}
                    Layout.fillWidth: true
                    contentItem: ColumnLayout {
                        QQC2.Label {
                            text: i18n("Avatar")
                            Layout.fillWidth: true
                        }

                        RowLayout {
                            QQC2.RoundButton {
                                id: avatarUpload
                                icon.name: 'download'
                                property var fileDialog: null;
                                Layout.alignment: Qt.AlignHCenter
                                onClicked: {
                                    if (fileDialog !== null) {
                                        return;
                                    }

                                    fileDialog = openFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

                                    fileDialog.chosen.connect(function(receivedSource) {
                                        avatarUpload.fileDialog = null;
                                        if (!receivedSource) {
                                            return;
                                        }
                                        account.identity.avatarUrl = receivedSource;
                                    });
                                    fileDialog.onRejected.connect(function() {
                                        avatarUpload.fileDialog = null;
                                    });
                                    fileDialog.open();
                                }
                            }
                            QQC2.Label {
                                Layout.alignment: Qt.AlignHCenter
                                text: i18n('PNG, GIF or JPG. At most 2 MB. Will be downscaled to 1500x500px')
                                wrapMode: Text.WordWrap
                            }
                        }

                        Kirigami.LinkButton {
                            text: i18n('Delete')
                            color: Kirigami.Theme.negativeTextColor
                        }
                    }
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCheckDelegate {
                    text: i18n("Require follow requests")
                    checked: account.identity.locked
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormCheckDelegate {
                    text: i18n("This is a bot account")
                    checked: account.identity.bot
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormCheckDelegate {
                    text: i18n("Suggest account to others")
                    checked: account.identity.discoverable
                }
            }
        }
    }

    footer: QQC2.ToolBar {
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18n("Apply")
                icon.name: 'dialog-ok'
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: account.saveAccount(newIdentity)
            }
        }
    }
}
