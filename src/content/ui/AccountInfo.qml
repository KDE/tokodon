// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQml.Models 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import org.kde.kmasto 1.0

TimelinePage {
    id: accountInfo
    isProfile: true
    listViewHeader: QQC2.Pane {
        width: parent.width
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        topPadding: 0

        contentItem: ColumnLayout {
            spacing: 0
            Item {
                Layout.preferredHeight: Kirigami.Units.gridUnit * 6
                Layout.fillWidth: true
                clip: true
                Image {
                    anchors.centerIn: parent
                    source: model.identity.backgroundUrl
                    fillMode: Image.PreserveAspectFit
                    visible: model.identity.backgroundUrl
                }
            }

            RowLayout {
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Item {
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 5
                    Layout.alignment: Qt.AlignBottom
                    Image {
                        anchors {
                            left: parent.left
                            bottom: parent.bottom
                        }
                        source: model.identity.avatarUrl
                        width: Kirigami.Units.gridUnit * 5
                        height: Kirigami.Units.gridUnit * 5
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                QQC2.Button {
                    id: followButton
                    text: {
                        if (model.identity.relationship && model.identity.relationship.requested) {
                          return i18n("Requested");
                        }
                        if (model.identity.relationship && model.identity.relationship.following) {
                          return i18n("Following");
                        }
                        return i18n("Follow");
                    }
                    Layout.alignment: Qt.AlignBottom
                    onClicked: {
                        if (model.identity.relationship.requested
                            || model.identity.relationship.following) {
                            model.unfollowAccount();
                        } else {
                            model.followAccount();
                        }
                    }
                }
            }

            Kirigami.Heading {
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                text: model.identity.displayName
            }
            Kirigami.Heading {
                level: 3
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                text: model.identity.account
            }
            Kirigami.Separator {
                Layout.fillWidth: true
                visible: accountInfo.model.identity.fields && accountInfo.model.identity.fields.length > 0
            }
            Repeater {
                model: accountInfo.model.identity.fields
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    RowLayout {
                        spacing: 0
                        QQC2.Pane {
                            contentItem: QQC2.Label {
                                text: modelData.name
                            }
                            Layout.minimumWidth: Kirigami.Units.gridUnit * 7
                            Layout.maximumWidth: Kirigami.Units.gridUnit * 7
                            Kirigami.Theme.colorSet: Kirigami.Theme.View
                            leftPadding: Kirigami.Units.largeSpacing
                            rightPadding: Kirigami.Units.largeSpacing
                            bottomPadding: 0
                            topPadding: 0
                        }

                        QQC2.TextArea {
                            Layout.fillWidth: true
                            readOnly: true
                            background: Rectangle {
                                color: modelData.verified_at !== null ? Kirigami.Theme.positiveBackgroundColor : Kirigami.Theme.backgroundColor
                            }
                            textFormat: TextEdit.RichText
                            text: modelData.value
                            onLinkActivated: Qt.openUrlExternally(link)
                        }
                    }
                    Kirigami.Separator {
                        Layout.fillWidth: true
                    }
                }
            }
            QQC2.Label {
                text: accountInfo.model.identity.bio
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                wrapMode: Text.WordWrap
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing

                QQC2.Label {
                    Layout.fillWidth: true
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    text: i18n("%1 toots", model.identity.statusesCount)
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    text: i18n("%1 followers", model.identity.followersCount)
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    text: i18n("%1 following", model.identity.followingCount)
                }
            }
            Kirigami.Separator {
                Layout.fillWidth: true
            }
        }
    }
}
