// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.kirigamiaddons.formcard 1 as FormCard
import Qt5Compat.GraphicalEffects
import QtQuick.Controls 2 as QQC2
import QtQml.Models
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.tokodon

TimelinePage {
    id: accountInfo

    required property string accountId

    property var postsBar

    readonly property var currentIndex: postsBar ? postsBar.currentIndex : 0
    readonly property bool onPostsTab: accountInfo.currentIndex === 0
    readonly property bool onRepliesTab: accountInfo.currentIndex === 1
    readonly property bool onMediaTab: accountInfo.currentIndex === 2

    readonly property bool canExcludeBoosts: accountInfo.onPostsTab || accountInfo.onRepliesTab
    property alias excludeBoosts: model.excludeBoosts

    readonly property bool largeScreen: width > Kirigami.Units.gridUnit * 25

    model: AccountModel {
        id: model

        accountId: accountInfo.accountId

        currentTab: accountInfo.currentIndex
    }

    listViewHeader: QQC2.Pane {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        width: parent.width

        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        topPadding: 0

        contentItem: Loader {
            active: model.identity
            sourceComponent: ColumnLayout {
                spacing: 0

                QQC2.Control {
                    Layout.fillWidth: true

                    background: Item {
                        Item {
                            anchors.fill: parent

                            Rectangle {
                                anchors.fill: parent
                                color: avatar.color
                                opacity: 0.2

                            }
                            Kirigami.Icon {
                                visible: source
                                scale: 1.8
                                anchors.fill: parent

                                source: model.identity.backgroundUrl

                                implicitWidth: 512
                                implicitHeight: 512
                            }

                            layer.enabled: true
                            layer.effect: HueSaturation {
                                cached: true

                                saturation: 1.9

                                layer {
                                    enabled: true
                                    effect: FastBlur {
                                        cached: true
                                        radius: 100
                                    }
                                }
                            }
                        }

                        Rectangle {
                            anchors.fill: parent
                            gradient: Gradient {
                                GradientStop { position: -1.0; color: "transparent" }
                                GradientStop { position: 1.0; color: Kirigami.Theme.backgroundColor }
                            }
                        }
                    }

                    QQC2.Control {
                        visible: accountInfo.model.identity.relationship && accountInfo.model.identity.relationship.followedBy

                        anchors {
                            right: parent.right
                            rightMargin: Kirigami.Units.smallSpacing
                            top: parent.top
                            topMargin: Kirigami.Units.smallSpacing
                        }

                        contentItem: QQC2.Label {
                            text: i18n("Follows you")
                            color: '#fafafa'
                        }

                        background: Rectangle {
                            radius: 3
                            color: '#090b0d'
                        }
                    }

                    contentItem: RowLayout {
                        RowLayout {
                            Layout.maximumWidth: Kirigami.Units.gridUnit * 30
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter

                            Kirigami.ShadowedRectangle {
                                Layout.margins: accountInfo.largeScreen ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
                                Layout.preferredWidth: accountInfo.largeScreen ? Kirigami.Units.gridUnit * 5 : Kirigami.Units.gridUnit * 3
                                Layout.preferredHeight: accountInfo.largeScreen ? Kirigami.Units.gridUnit * 5 : Kirigami.Units.gridUnit * 3

                                color: Kirigami.Theme.backgroundColor
                                radius: width

                                shadow {
                                    size: 15
                                    xOffset: 5
                                    yOffset: 5
                                    color: Qt.rgba(0, 0, 0, 0.2)
                                }

                                Components.Avatar {
                                    id: avatar

                                    height: parent.height
                                    width: height

                                    name: model.identity.displayName
                                    source: model.identity.avatarUrl
                                    imageMode: Components.Avatar.ImageMode.AdaptiveImageOrInitals
                                }
                            }

                            ColumnLayout {
                                spacing: Kirigami.Units.smallSpacing

                                Layout.leftMargin: Kirigami.Units.largeSpacing
                                Layout.rightMargin: Kirigami.Units.largeSpacing
                                Layout.fillWidth: true

                                QQC2.Label {
                                    Layout.fillWidth: true
                                    text: model.identity.displayNameHtml
                                    font.bold: true
                                    font.pixelSize: 24
                                    maximumLineCount: 2
                                    wrapMode: Text.Wrap
                                    elide: Text.ElideRight

                                }

                                QQC2.TextArea {
                                    text: "@" + model.identity.account
                                    textFormat: TextEdit.PlainText
                                    readOnly: true
                                    background: null
                                    font.pixelSize: 18

                                    leftPadding: 0
                                    rightPadding: 0
                                    topPadding: 0

                                    Layout.fillWidth: true
                                }

                                Kirigami.ActionToolBar {
                                    id: toolbar

                                    flat: false
                                    alignment: Qt.AlignLeft

                                    actions: [
                                        Kirigami.Action {
                                            icon.name: {
                                                if (model.identity.relationship && model.identity.relationship.following) {
                                                    return "list-remove-user";
                                                }
                                                return "list-add-user";
                                            }

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
                                            displayHint: Kirigami.DisplayHint.IconOnly
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
                                            displayHint: Kirigami.DisplayHint.IconOnly
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
                                                    model.account.unmuteAccount(model.identity);
                                                } else {
                                                    model.account.muteAccount(model.identity);
                                                }
                                            }
                                        },
                                        Kirigami.Action {
                                            icon.name: "im-ban-kick-user"
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
                                            visible: model.isSelf
                                            text: i18n("Edit Profile")
                                            onTriggered: pageStack.push(Qt.createComponent("org.kde.tokodon", "ProfileEditor"), {
                                                                account: model.account
                                                            }, {
                                                                title: i18n("Account editor")
                                                            })
                                        },
                                        Kirigami.Action {
                                            icon.name: "settings-configure"
                                            visible: model.isSelf
                                            text: i18n("Settings")
                                            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "SettingsPage"), {}, { title: i18n("Configure") })
                                        },
                                        Kirigami.Action {
                                            icon.name: "list-add-user"
                                            visible: model.isSelf
                                            text: i18n("Follow Requests")
                                            onTriggered: pageStack.push(socialGraphComponent, { name: "request" });
                                        },
                                        Kirigami.Action {
                                            icon.name: "microphone-sensitivity-muted"
                                            visible: model.isSelf
                                            text: i18n("Muted Accounts")
                                            onTriggered: pageStack.push(socialGraphComponent, { name: "mutes" });
                                        },
                                        Kirigami.Action {
                                            icon.name: "cards-block"
                                            visible: model.isSelf
                                            text: i18n("Blocked Accounts")
                                            onTriggered: pageStack.push(socialGraphComponent, { name: "blocks" });
                                        },
                                        Kirigami.Action {
                                            icon.name: "favorite"
                                            visible: model.isSelf
                                            text: i18n("Featured Accounts")
                                            onTriggered: pageStack.push(socialGraphComponent, { name: "featured" });
                                        },
                                        Kirigami.Action {
                                            icon.name: "edit-copy"
                                            text: i18n("Copy Link to This Profile")
                                            onTriggered: {
                                                Clipboard.saveText(model.identity.url)
                                                applicationWindow().showPassiveNotification(i18n("Post link copied."));
                                            }
                                        },
                                        ShareAction {
                                            id: shareAction

                                            inputData: {
                                                'urls': [model.identity.url.toString()],
                                                'title': "Profile",
                                            }
                                        }
                                    ]
                                }
                            }
                        }
                    }
                }

                FormCard.FormCard {
                    visible: accountInfo.model.identity.fields.length > 0

                    Repeater {
                        model: accountInfo.model.identity.fields
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 0
                            FormCard.FormDelegateSeparator { visible: index !== 0 }

                            FormCard.AbstractFormDelegate {
                                topPadding: 0
                                bottomPadding: 0

                                background: Rectangle {
                                    color: modelData.verified_at !== null ? Kirigami.Theme.positiveBackgroundColor : "transparent"
                                }

                                contentItem: RowLayout {
                                    spacing: 0

                                    QQC2.Label {
                                        text: modelData.name
                                        wrapMode: Text.WordWrap

                                        topPadding: Kirigami.Units.smallSpacing
                                        bottomPadding: Kirigami.Units.smallSpacing

                                        Layout.minimumWidth: Kirigami.Units.gridUnit * 7
                                        Layout.maximumWidth: Kirigami.Units.gridUnit * 7
                                    }

                                    QQC2.TextArea {
                                        Layout.fillWidth: true
                                        readOnly: true
                                        wrapMode: Text.WordWrap
                                        background: null
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
                    }
                }

                FormCard.FormCard {
                    visible: accountInfo.model.identity.relationship

                    Layout.topMargin: Kirigami.Units.largeSpacing

                    FormCard.AbstractFormDelegate {
                        background: null

                        contentItem: ColumnLayout {
                            spacing: 0
                            RowLayout {
                                Layout.fillWidth: true

                                Kirigami.Heading {
                                    Layout.fillWidth: true
                                    text: i18n("Note:")
                                    level: 5
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
                                Layout.fillWidth: true
                                background: null
                                placeholderText: i18n("Click to add a note")
                                textFormat: TextEdit.PlainText
                                leftPadding: 0
                                rightPadding: 0
                                text: accountInfo.model.identity.relationship ? accountInfo.model.identity.relationship.note : ''
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
                        }
                    }
                }


                FormCard.FormCard {
                    id: bioCard

                    Layout.topMargin: Kirigami.Units.largeSpacing

                    FormCard.AbstractFormDelegate {
                        background: null
                        contentItem: QQC2.TextArea {
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
                            onLinkActivated: (link) => applicationWindow().navigateLink(link, true)
                            onHoveredLinkChanged: if (hoveredLink.length > 0) {
                                applicationWindow().hoverLinkIndicator.text = hoveredLink;
                            } else {
                                applicationWindow().hoverLinkIndicator.text = "";
                            }

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing

                    implicitHeight: chips.implicitHeight
                    implicitWidth: chips.implicitWidth + chips.anchors.leftMargin + chips.anchors.rightMargin

                    RowLayout {
                        id: chips

                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            leftMargin: bioCard.cardWidthRestricted ? Math.round((bioCard.width - bioCard.maximumWidth) / 2) : 0
                            rightMargin: bioCard.cardWidthRestricted ? Math.round((bioCard.width - bioCard.maximumWidth) / 2) : 0
                        }

                        Kirigami.Chip {
                            closable: false
                            enabled: false

                            text: i18ncp("@label User's number of statuses", "<b>%1</b> post", "<b>%1</b> posts", model.identity.statusesCount)
                        }

                        Kirigami.Chip {
                            closable: false
                            checkable: false

                            onClicked: {
                                pageStack.push(socialGraphComponent, {
                                    name: "followers",
                                    accountId: accountId,
                                });
                            }
                            text: i18ncp("@label User's number of followers", "<b>%1</b> follower", "<b>%1</b> followers", model.identity.followersCount)
                        }

                        Kirigami.Chip {
                            closable: false
                            checkable: false

                            onClicked: {
                                pageStack.push(socialGraphComponent, {
                                    name: "following",
                                    accountId: accountId,
                                });
                            }
                            text: i18ncp("@label User's number of followed accounts", "<b>%1</b> follows", "<b>%1</b> following", model.identity.followingCount)
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }

                QQC2.TabBar {
                    id: bar

                    // Hack to disable the qqc2-desktop-style scrolling behavior.
                    // This bar is on a scrollable page, you will eventually run into this tab bar which is annoying.
                    background: null

                    enabled: !accountInfo.model.loading

                    Component.onCompleted: accountInfo.postsBar = bar

                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true

                    QQC2.TabButton {
                        text: i18nc("@item:inmenu Profile Post Filter", "Posts")
                        implicitWidth: bar.width / 3
                    }
                    QQC2.TabButton {
                        text: i18nc("@item:inmenu Profile Post Filter", "Posts && Replies")
                        implicitWidth: bar.width / 3
                    }
                    QQC2.TabButton {
                        text: i18nc("@item:inmenu Profile Post Filter", "Media")
                        implicitWidth: bar.width / 3
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

                            checked: accountInfo.excludeBoosts
                            enabled: accountInfo.canExcludeBoosts && !accountInfo.model.loading

                            onToggled: accountInfo.excludeBoosts = checked
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
