// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQml.Models 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import org.kde.kmasto 1.0

TimelinePage {
    id: accountInfo

    required property string accountId

    property var postsBar

    readonly property var currentIndex: postsBar ? postsBar.currentIndex : 0
    readonly property bool onPostsTab: accountInfo.currentIndex === 0
    readonly property bool onRepliesTab: accountInfo.currentIndex === 1
    readonly property bool onMediaTab: accountInfo.currentIndex === 2

    readonly property bool canExcludeBoosts: accountInfo.onPostsTab || accountInfo.onRepliesTab
    property bool excludeBoosts: false

    model: AccountModel {
        id: model

        accountId: accountInfo.accountId

        excludeReplies: !accountInfo.onRepliesTab
        excludeBoosts: accountInfo.excludeBoosts || accountInfo.onMediaTab
        excludePinned: !accountInfo.onPostsTab
        onlyMedia: accountInfo.onMediaTab
    }

    listViewHeader: QQC2.Pane {
        width: parent.width
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        topPadding: 0

        contentItem: Loader {
            active: model.identity
            sourceComponent: ColumnLayout {
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
                                icon.name: "view-hidden"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.identity.relationship && model.identity.relationship.following && !model.isSelf
                                text: {
                                    if (model.identity.relationship && model.identity.relationship.showingReblogs) {
                                        return i18n("Hide Boosts from %1", '@' + model.identity.account);
                                    } else {
                                        return i18n("Stop Hiding Boosts from %1", '@' + model.identity.account);
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
                                icon.name: "favorite"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.identity.relationship && !model.isSelf
                                text: {
                                    if (model.identity.relationship && model.identity.relationship.endorsed) {
                                        return i18n("Stop Featuring on Profile");
                                    } else {
                                        return i18n("Feature on Profile");
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
                                icon.name: "dialog-cancel"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.identity.relationship && !model.isSelf
                                text: {
                                    if (model.identity.relationship && model.identity.relationship.muting) {
                                        return i18n("Stop Muting");
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
                                icon.name: "im-ban-kick-user"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.identity.relationship && !model.isSelf
                                text: {
                                    if (model.identity.relationship && model.identity.relationship.blocking) {
                                        return i18n("Stop Blocking");
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
                            },
                            Kirigami.Action {
                                icon.name: "user-group-properties"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.isSelf
                                text: i18n("Edit Profile")
                                onTriggered: pageStack.push('qrc:/content/ui/Settings/ProfileEditor.qml', {
                                                    account: model.account
                                                }, {
                                                    title: i18n("Account editor")
                                                })
                            },
                            Kirigami.Action {
                                icon.name: "settings-configure"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.isSelf
                                text: i18n("Settings")
                                onTriggered: pageStack.pushDialogLayer('qrc:/content/ui/Settings/SettingsPage.qml', {}, { title: i18n("Configure") })
                            },
                            Kirigami.Action {
                                icon.name: "list-add-user"
                                displayHint: Kirigami.DisplayHint.AlwaysHide
                                visible: model.isSelf
                                text: i18n("Follow Requests")
                                onTriggered: pageStack.push(socialGraphComponent, { name: "request" });
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
                QQC2.TextArea {
                    text: "@" + model.identity.account
                    color: Kirigami.Theme.disabledTextColor
                    textFormat: TextEdit.PlainText
                    readOnly: true
                    Layout.fillWidth: true
                    background: null
                    topPadding: 0
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
                    visible: noteField.visible
                }
                QQC2.TextArea {
                    text: accountInfo.model.identity.bio
                    textFormat: TextEdit.RichText
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.smallSpacing
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                    leftPadding: 0
                    rightPadding: 0
                    bottomPadding: 0
                    topPadding: 0
                    background: null
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }
                RowLayout {
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing

                    Kirigami.Chip {
                        closable: false
                        enabled: false

                        text: i18nc("@label User's number of statuses", "<b>%1</b> posts", model.identity.statusesCount)
                    }
                    Kirigami.Chip {
                        closable: false
                        enabled: false

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                pageStack.push(socialGraphComponent, {
                                    name: "followers",
                                    accountId: accountId,
                                });
                            }
                        }
                        text: i18nc("@label User's number of followers", "<b>%1</b> followers", model.identity.followersCount)
                    }
                    Kirigami.Chip {
                        closable: false
                        enabled: false

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                pageStack.push(socialGraphComponent, {
                                    name: "following",
                                    accountId: accountId,
                                });
                            }
                        }
                        text: i18nc("@label User's number of followed accounts", "<b>%1</b> following", model.identity.followingCount)
                    }
                }
                QQC2.TabBar {
                    id: bar

                    Component.onCompleted: accountInfo.postsBar = bar

                    Layout.alignment: Qt.AlignHCenter

                    QQC2.TabButton {
                        text: i18nc("@item:inmenu Profile Post Filter", "Posts")
                    }
                    QQC2.TabButton {
                        text: i18nc("@item:inmenu Profile Post Filter", "Posts && Replies")
                    }
                    QQC2.TabButton {
                        text: i18nc("@item:inmenu Profile Post Filter", "Media")
                    }
                }
                Rectangle {
                    Layout.fillWidth: true

                    implicitHeight: extraLayout.implicitHeight + Kirigami.Units.largeSpacing * 2

                    Kirigami.Theme.inherit: false
                    Kirigami.Theme.colorSet: Kirigami.Theme.Header

                    color: Kirigami.Theme.backgroundColor

                    RowLayout {
                        id: extraLayout

                        anchors {
                            fill: parent

                            topMargin: Kirigami.Units.largeSpacing
                            leftMargin: Kirigami.Units.largeSpacing
                            rightMargin: Kirigami.Units.largeSpacing
                            bottomMargin: Kirigami.Units.largeSpacing
                        }

                        QQC2.Switch {
                            text: i18nc("@option:check", "Hide boosts")

                            onToggled: accountInfo.excludeBoosts = checked

                            enabled: accountInfo.canExcludeBoosts
                        }
                    }
                }
                Kirigami.Separator {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
