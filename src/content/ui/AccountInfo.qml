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
    isProfile: true
    listViewHeader: ColumnLayout {
        width: parent.width
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
            text: model.identity.account
        }
        Repeater {
            model: model.identity.fields
            QQC2.Label {
                Layout.fillWidth: true
                text: `${name} ${value}`
            }
        }
        QQC2.Label {
            text: model.identity.bio
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            wrapMode: Text.WordWrap
        }
        RowLayout {
            Layout.fillWidth: true

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
