// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kmasto 1.0
import Qt.labs.platform 1.1

Kirigami.ScrollablePage {
    property var account

    property Identity identity: Identity {
        id: newIdentity
    }

    title: i18n('Profile Editor')

    Component {
        id: openFileDialog
        FileDialog {
            signal chosen(string path)
            title: i18n("Please choose a file")
            folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
            onAccepted: chosen(file)
        }
    }

    Kirigami.FormLayout {
        QQC2.TextField {
            Kirigami.FormData.label: i18n("Display Name:") 
            text: account.identity.displayName
            onTextChanged: newIdentity.displayName = text
            Layout.maximumWidth: Kirigami.Units.gridUnit * 20
        }

        QQC2.TextArea {
            id: bioField
            Layout.preferredWidth: Kirigami.Units.gridUnit * 20
            Kirigami.FormData.label: i18n("Bio:") 
            text: account.identity.bio
            textFormat: TextEdit.RichText
        }

        RowLayout {
            implicitHeight: Kirigami.Units.gridUnit * 13
            Kirigami.Card {
                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 20
                Layout.maximumHeight: Kirigami.Units.gridUnit * 12
                banner {
                    source: 'file:///home/cschwan/Pictures/profile.png' //account.identity.backgroundUrl
                }
                Component.onCompleted: {
                    banner.Layout.preferredWidth = Kirigami.Units.gridUnit * 20
                    banner.Layout.preferredHeight = Kirigami.Units.gridUnit * 8
                    banner.Layout.maximumHeight = Kirigami.Units.gridUnit * 8
                    banner.fillMode = Image.PreserveAspectCrop
                }
                topPadding: 0
                leftPadding: 0
                rightPadding: 0
                bottomPadding: 0
                contentItem: QQC2.Pane {
                    background: Item {}
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

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignTop
                Kirigami.Heading {
                    level: 4
                    text: i18n("Header")
                }

                RowLayout {
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
                        text: i18n('PNG, GIF or JPG. At most 2 MB. Will be downscaled to 1500x500px')
                        wrapMode: Text.WordWrap
                    }
                }

                Kirigami.LinkButton {
                    text: i18n('Delete')
                    color: Kirigami.Theme.negativeTextColor
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                }

                Kirigami.Heading {
                    level: 4
                    text: i18n("Avatar")
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

        QQC2.CheckBox {
            text: i18n("Require follow requests")
            checked: account.identity.locked
        }

        QQC2.CheckBox {
            text: i18n("This is a bot account")
            checked: account.identity.bot
        }

        QQC2.CheckBox {
            text: i18n("Suggest account to others")
            checked: account.identity.discoverable
        }
    }

    footer: RowLayout {
        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18n('Save')
            icon.name: 'dialog-ok'
            Layout.margins: Kirigami.Units.smallSpacing
            onClicked: account.saveAccount(newIdentity)
        }
    }
}
