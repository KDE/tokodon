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
    type: TimelinePage.TimelineType.Profile
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
            Rectangle {
                Layout.preferredHeight: Kirigami.Units.gridUnit * 7
                Layout.fillWidth: true
                clip: true
                color: Kirigami.Theme.backgroundColor
                Kirigami.Theme.colorSet: Kirigami.Theme.View

                Image {
                    anchors.centerIn: parent
                    source: model.identity.backgroundUrl
                    fillMode: Image.PreserveAspectFit
                    visible: model.identity.backgroundUrl
                }

                QQC2.Control {
                    visible: accountInfo.model.identity.relationship && accountInfo.model.identity.relationship.followedBy
                    x: Kirigami.Units.smallSpacing
                    y: Kirigami.Units.smallSpacing
                    contentItem: QQC2.Label {
                        text: i18n("Follows you")
                        color: '#fafafa'
                    }

                    background: Rectangle {
                        radius: 3
                        color: '#090b0d'
                    }
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
                    Kirigami.Avatar {
                        anchors {
                            left: parent.left
                            bottom: parent.bottom
                        }
                        cache: true
                        source: model.identity.avatarUrl
                        width: Kirigami.Units.gridUnit * 5
                        height: Kirigami.Units.gridUnit * 5
                    }
                }
                Kirigami.ActionToolBar {
                    Layout.fillWidth: true
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                    Layout.alignment: Qt.AlignBottom
                    alignment: Qt.AlignRight
                    flat: false
                    actions: [
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.KeepVisible
                            text: {
                                if (model.identity.relationship && model.identity.relationship.requested) {
                                    return i18n("Requested");
                                }
                                if (model.identity.relationship && model.identity.relationship.following) {
                                    return i18n("Following");
                                }
                                return i18n("Follow");
                            }
                            onTriggered: {
                                if (model.identity.relationship.requested
                                    || model.identity.relationship.following) {
                                    model.account.unfollowAccount(model.identity);
                                } else {
                                    model.account.followAccount(model.identity);
                                }
                            }
                            visible: !model.isSelf
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.KeepVisible
                            icon.name: {
                                if (model.identity.relationship && model.identity.relationship.notifying) {
                                    return "notifications";
                                } else {
                                    return "notifications-disabled";
                                }
                            }

                            visible: model.identity.relationship && model.identity.relationship.following && !model.isSelf
                            tooltip: {
                                if (model.identity.relationship && model.identity.relationship.notifying) {
                                    return i18n("Stop notifying me when %1 posts", '@' + model.identity.account);
                                } else {
                                    return i18n("Notify me when %1 posts", '@' + model.identity.account);
                                }
                            }
                            onTriggered: {
                                if (model.identity.relationship && model.identity.relationship.notifying) {
                                    model.account.followAccount(model.identity, model.identity.relationship.showingReblogs, false);
                                } else {
                                    model.account.followAccount(model.identity, model.identity.relationship.showingReblogs, true);
                                }
                            }
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.AlwaysHide
                            visible: model.identity.relationship && model.identity.relationship.following && !model.isSelf
                            text: {
                                if (model.identity.relationship && model.identity.relationship.showingReblogs) {
                                    return i18n("Hide boosts from %1", '@' + model.identity.account);
                                } else {
                                    return i18n("Stop hiding boosts from %1", '@' + model.identity.account);
                                }
                            }
                            onTriggered: {
                                if (model.identity.relationship && model.identity.relationship.showingReblogs) {
                                    model.account.followAccount(model.identity, false, model.identity.relationship.notifying);
                                } else {
                                    model.account.followAccount(model.identity, true, model.identity.relationship.notifying);
                                }
                            }
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.AlwaysHide
                            visible: model.identity.relationship && !model.isSelf
                            text: {
                                if (model.identity.relationship && model.identity.relationship.endorsed) {
                                    return i18n("Stop featuring on profile");
                                } else {
                                    return i18n("Feature on profile");
                                }
                            }
                            onTriggered: {
                                if (model.identity.relationship && model.identity.relationship.endorsed) {
                                    model.account.unpin(model.identity);
                                } else {
                                    model.account.pin(model.identity);
                                }
                            }
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.AlwaysHide
                            visible: model.identity.relationship && !model.isSelf
                            text: {
                                if (model.identity.relationship && model.identity.relationship.muting) {
                                    return i18n("Stop muting");
                                } else {
                                    return i18n("Mute");
                                }
                            }
                            onTriggered: {
                                if (model.identity.relationship && model.identity.relationship.muting) {
                                    model.account.unmute(model.identity);
                                } else {
                                    model.account.mute(model.identity);
                                }
                            }
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.AlwaysHide
                            visible: model.identity.relationship && !model.isSelf
                            text: {
                                if (model.identity.relationship && model.identity.relationship.blocking) {
                                    return i18n("Stop blocking");
                                } else {
                                    return i18n("Block");
                                }
                            }
                            onTriggered: {
                                if (model.identity.relationship && model.identity.relationship.blocking) {
                                    model.account.unblock(model.identity);
                                } else {
                                    model.account.block(model.identity);
                                }
                            }
                        }
                    ]
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
                text: '@' + model.identity.account
                color: Kirigami.Theme.disabledTextColor
            }
            Repeater {
                model: accountInfo.model.identity.fields
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    Kirigami.Separator {
                        Layout.fillWidth: true
                    }
                    RowLayout {
                        spacing: 0
                        QQC2.Pane {
                            contentItem: QQC2.Label {
                                text: modelData.name
                                wrapMode: Text.WordWrap
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
                            Layout.fillHeight: true
                            readOnly: true
                            wrapMode: Text.WordWrap
                            background: Rectangle {
                                color: modelData.verified_at !== null ? Kirigami.Theme.positiveBackgroundColor : Kirigami.Theme.backgroundColor
                            }
                            textFormat: TextEdit.RichText
                            text: modelData.value
                            onLinkActivated: Qt.openUrlExternally(link)
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                            }
                        }
                    }
                }
            }
            Kirigami.Separator {
                Layout.fillWidth: true
            }
            RowLayout {
                visible: accountInfo.model.identity.relationship
                Kirigami.Heading {
                    Layout.fillWidth: true
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.smallSpacing
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                    text: i18n("Note")
                    level: 4
                }

                QQC2.Control {
                    id: savedNotification
                    visible: false
                    contentItem: QQC2.Label {
                        text: i18n("Saved")
                        color: Kirigami.Theme.positiveTextColor
                    }

                    background: Rectangle {
                        radius: 3
                        color: Kirigami.Theme.positiveBackgroundColor
                    }
                }
            }
            QQC2.TextArea {
                id: noteField
                visible: accountInfo.model.identity.relationship
                Layout.fillWidth: true
                background: Item {}
                placeholderText: i18n("Click to add a note")
                textFormat: TextEdit.PlainText
                text: accountInfo.model.identity.relationship ? accountInfo.model.identity.relationship.note : ''
                Layout.leftMargin: 0
                Layout.rightMargin: 0
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                property string lastSavedText: ''
                onActiveFocusChanged: {
                    lastSavedText = text;
                    if (activeFocus) {
                        autoSaveTimer.start()
                    } else {
                        accountInfo.model.account.addNote(accountInfo.model.identity, text);
                        savedNotification.visible = true;
                        savedNotificationTimer.restart();
                        lastSavedText = text;
                        autoSaveTimer.stop()
                    }
                }
                Timer {
                    id: autoSaveTimer
                    running: false
                    repeat: true
                    interval: 5000
                    onTriggered: if (noteField.lastSavedText !== noteField.text) {
                        accountInfo.model.account.addNote(accountInfo.model.identity, noteField.text);
                        savedNotification.visible = true;
                        noteField.lastSavedText = noteField.text;
                        savedNotificationTimer.restart();
                    }
                }

                Timer {
                    id: savedNotificationTimer
                    running: false
                    interval: 5000
                    onTriggered: savedNotification.visible = false
                }
            }
            Kirigami.Separator {
                Layout.fillWidth: true
            }
            QQC2.Label {
                text: accountInfo.model.identity.bio
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
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
