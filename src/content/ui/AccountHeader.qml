// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.kirigamiaddons.labs.components as Labs
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

QQC2.Pane {
    id: root

    required property var identity
    required property bool isSelf

    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0
    topPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    function getBar() {
       return bar;
    }

    contentItem: ColumnLayout {
        id: layout

        spacing: 0

        Kirigami.InlineMessage {
            id: migrationMessage

            type: Kirigami.MessageType.Information
            position: Kirigami.InlineMessage.Position.Header
            visible: root.identity.moved
            showCloseButton: false
            text: i18nc("@info %1 is a display name", "%1 has moved to a new account:", root.identity.displayName)

            Layout.preferredHeight: migrationLayout.implicitHeight + topPadding + bottomPadding
            Layout.fillWidth: true

            contentItem: ColumnLayout {
                id: migrationLayout

                anchors {
                    fill: parent
                    topMargin: migrationMessage.topPadding
                    bottomMargin: migrationMessage.bottomPadding
                    leftMargin: migrationMessage.leftPadding
                    rightMargin: migrationMessage.rightPadding
                }

                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: migrationMessage.text

                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: 0

                    Layout.fillWidth: true

                    InlineIdentityInfo {
                        identity: root.identity.moved
                        secondary: false

                        Layout.fillWidth: true
                    }

                    QQC2.Button {
                        text: i18nc("@action:button", "Go to Profile")
                        onClicked: Navigation.openAccount(root.identity.moved.id)
                    }
                }
            }
        }

        QQC2.Control {
            id: avatarControl

            Layout.fillWidth: true

            background: Item {
                Item {
                    anchors.fill: parent

                    Rectangle {
                        anchors.fill: parent
                        color: avatar.color
                        opacity: 0.2

                    }
                    Image {
                        visible: source
                        anchors.fill: parent

                        source: root.identity.backgroundUrl
                    }

                    layer.enabled: true
                    layer.effect: MultiEffect {
                        saturation: 1.9

                        layer {
                            enabled: true
                            effect: MultiEffect {
                                blurEnabled: true
                                autoPaddingEnabled: false
                                blurMax: 100
                                blur: 1.0
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

            RowLayout {
                anchors {
                    right: parent.right
                    rightMargin: Kirigami.Units.smallSpacing
                    top: parent.top
                    topMargin: Kirigami.Units.smallSpacing
                }

                spacing: Kirigami.Units.mediumSpacing

                QQC2.Control {
                    visible: root.identity.relationship && root.identity.relationship.followedBy

                    contentItem: QQC2.Label {
                        text: root.identity.relationship && root.identity.relationship.following && root.identity.relationship.followedBy ? i18nc("'Mutual' is the common English word used in social media platforms describe people who follow you back. This the same meaning as 'mutual friends', for example.", "Mutual") : i18n("Follows you")
                        color: '#fafafa'
                    }

                    background: Rectangle {
                        radius: 3
                        color: '#090b0d'
                    }
                }

                QQC2.Control {
                    visible: root.identity.bot

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing
                        Kirigami.Icon {
                            source: "automated-tasks"
                            Layout.preferredWidth: Kirigami.Units.iconSizes.sizeForLabels
                            Layout.preferredHeight: Kirigami.Units.iconSizes.sizeForLabels
                        }
                        QQC2.Label {
                            text: i18n("Bot")
                            color: '#fafafa'
                        }
                    }

                    background: Rectangle {
                        radius: 3
                        color: '#090b0d'
                    }
                }
            }

            contentItem: RowLayout {
                spacing: 0

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

                        Components.AvatarButton {
                            id: avatar

                            height: parent.height
                            width: height

                            name: root.identity.displayName
                            source: root.identity.avatarUrl
                            imageMode: Components.Avatar.ImageMode.AdaptiveImageOrInitals

                            property Labs.AlbumModelItem album: Labs.AlbumModelItem {
                                type: Labs.AlbumModelItem.Image
                                source: root.identity.avatarUrl
                            }

                            onClicked: Navigation.openFullScreenImage([album], root.identity, 0);
                        }
                    }

                    ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        Layout.fillWidth: true

                        ColumnLayout {
                            spacing: 0

                            Layout.fillWidth: true

                            QQC2.Label {
                                Layout.fillWidth: true
                                text: root.identity.displayNameHtml
                                font.bold: true
                                font.pixelSize: 24
                                maximumLineCount: 2
                                wrapMode: Text.Wrap
                                elide: Text.ElideRight
                            }

                            QQC2.TextArea {
                                text: "@" + root.identity.account
                                textFormat: TextEdit.PlainText
                                wrapMode: TextEdit.Wrap
                                readOnly: true
                                background: null
                                font.pixelSize: 18

                                leftPadding: 0
                                rightPadding: 0
                                topPadding: 0

                                Layout.fillWidth: true
                            }
                        }

                        Kirigami.ActionToolBar {
                            id: toolbar

                            flat: false
                            alignment: Qt.AlignLeft

                            KQuickControlsAddons.Clipboard { id: clipboard }

                            actions: [
                                Kirigami.Action {
                                    icon.name: {
                                if (root.identity.relationship && root.identity.relationship.following) {
                                    return "list-remove-user";
                                }
                                return "list-add-user";
                            }

                            text: {
                                if (root.identity.relationship && root.identity.relationship.requested) {
                                    return i18n("Follow Requested");
                                }
                                if (root.identity.relationship && root.identity.relationship.following) {
                                    return i18n("Unfollow");
                                }
                                return i18n("Follow");
                            }
                            onTriggered: {
                                if (root.identity.relationship.requested
                                    || root.identity.relationship.following) {
                                    AccountManager.selectedAccount.unfollowAccount(root.identity);
                                } else {
                                    AccountManager.selectedAccount.followAccount(root.identity);
                                }
                            }
                            visible: !root.isSelf
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.IconOnly
                            icon.name: {
                                if (root.identity.relationship && root.identity.relationship.notifying) {
                                    return "notifications-disabled";
                                } else {
                                    return "notifications";
                                }
                            }

                            visible: root.identity.relationship && root.identity.relationship.following && !root.isSelf
                            tooltip: {
                                if (root.identity.relationship && root.identity.relationship.notifying) {
                                    return i18n("Stop notifying me when %1 posts.", '@' + root.identity.account);
                                } else {
                                    return i18n("Notify me when %1 posts.", '@' + root.identity.account);
                                }
                            }
                            onTriggered: {
                                if (root.identity.relationship && root.identity.relationship.notifying) {
                                    AccountManager.selectedAccount.followAccount(root.identity, root.identity.relationship.showingReblogs, false);
                                } else {
                                    AccountManager.selectedAccount.followAccount(root.identity, root.identity.relationship.showingReblogs, true);
                                }
                            }
                        },
                        Kirigami.Action {
                            displayHint: Kirigami.DisplayHint.IconOnly
                            icon.name: "view-barcode-qr-symbolic"

                            tooltip: i18nc("@info:tooltip", "Show a QR code for this account")
                            onTriggered: {
                                const code = Qt.createComponent("org.kde.tokodon", "QrCodeMaximizeComponent").createObject(root.QQC2.Overlay.overlay, {
                                    url: root.identity.url,
                                    title: root.identity.displayName,
                                    subtitle: '@' + root.identity.account
                                });
                                code.open();
                            }
                        },
                        Kirigami.Action {
                            icon.name: "list-add"
                            visible: !root.isSelf
                            text: i18n("Mention…")
                            onTriggered: Navigation.openComposer("@" + root.identity.account + " ")
                        },
                        Kirigami.Action {
                            icon.name: "view-conversation-balloon-symbolic"
                            visible: !root.isSelf
                            text: i18n("Start a Conversation…")
                            onTriggered: Navigation.openConversation(root.identity.account)
                        },
                        Kirigami.Action {
                            icon.name: "view-hidden"
                            displayHint: Kirigami.DisplayHint.IconOnly
                            visible: root.identity.relationship && root.identity.relationship.following && !root.isSelf
                            text: {
                                if (root.identity.relationship && root.identity.relationship.showingReblogs) {
                                    return i18n("Hide Boosts from %1", '@' + root.identity.account);
                                } else {
                                    return i18n("Show Boosts from %1", '@' + root.identity.account);
                                }
                            }
                            onTriggered: {
                                if (root.identity.relationship && root.identity.relationship.showingReblogs) {
                                    AccountManager.selectedAccount.followAccount(root.identity, false, root.identity.relationship.notifying);
                                } else {
                                    AccountManager.selectedAccount.followAccount(root.identity, true, root.identity.relationship.notifying);
                                }
                            }
                        },
                        Kirigami.Action {
                            icon.name: "favorite"
                            visible: root.identity.relationship && !root.isSelf
                            text: {
                                if (root.identity.relationship && root.identity.relationship.endorsed) {
                                    return i18n("Stop Featuring This Profile");
                                } else {
                                    return i18n("Feature This Profile");
                                }
                            }
                            onTriggered: {
                                if (root.identity.relationship && root.identity.relationship.endorsed) {
                                    AccountManager.selectedAccount.unpin(root.identity);
                                } else {
                                    AccountManager.selectedAccount.pin(root.identity);
                                }
                            }
                        },
                        Kirigami.Action {
                            icon.name: "dialog-cancel"
                            visible: root.identity.relationship && !root.isSelf
                            text: {
                                if (root.identity.relationship && root.identity.relationship.muting) {
                                    return i18n("Unmute");
                                } else {
                                    return i18n("Mute");
                                }
                            }
                            onTriggered: {
                                if (root.identity.relationship && root.identity.relationship.muting) {
                                    AccountManager.selectedAccount.unmuteAccount(root.identity);
                                } else {
                                    AccountManager.selectedAccount.muteAccount(root.identity);
                                }
                            }
                        },
                        Kirigami.Action {
                            icon.name: "im-ban-kick-user"
                            visible: root.identity.relationship && !root.isSelf
                            text: {
                                if (root.identity.relationship && root.identity.relationship.blocking) {
                                    return i18n("Unblock");
                                } else {
                                    return i18n("Block");
                                }
                            }
                            onTriggered: {
                                if (root.identity.relationship && root.identity.relationship.blocking) {
                                    AccountManager.selectedAccount.unblock(root.identity);
                                } else {
                                    AccountManager.selectedAccount.block(root.identity);
                                }
                            }
                        },
                        Kirigami.Action {
                            icon.name: "dialog-warning-symbolic"
                            visible: !root.isSelf
                            text: i18nc("@action:inmenu Report this post", "Report…");
                            onTriggered: Navigation.reportUser(root.identity)
                        },
                        Kirigami.Action {
                            icon.name: "user-group-properties"
                            visible: root.isSelf
                            text: i18n("Edit Profile")
                            onTriggered: pageStack.push(Qt.createComponent("org.kde.tokodon", "EditProfilePage"), {
                                account: AccountManager.selectedAccount
                            }, {
                                title: i18n("Account editor")
                            })
                        },
                        Kirigami.Action {
                            visible: root.isSelf
                            fromQAction: (toolbar.QQC2.ApplicationWindow.window as StatefulApp.StatefulWindow)?.application.action('options_configure') ?? null
                        },
                        Kirigami.Action {
                            icon.name: "favorite"
                            visible: root.isSelf
                            text: i18n("Featured Users")
                            onTriggered: pageStack.push(socialGraphComponent, { name: "featured" });
                        },
                        Kirigami.Action {
                            icon.name: "resource-calendar-insert"
                            visible: root.isSelf
                            text: i18nc("@action:inmenu", "Scheduled Posts")
                            onTriggered: {
                                const page = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "ScheduledPostsPage"), { drafts: false });
                                page.opened.connect(function(id) {
                                    const composer = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                                        purpose: StatusComposer.New,
                                    });
                                    composer.openDraft(id);
                                });
                            }
                        },
                        Kirigami.Action {
                            icon.name: "document-open-folder-symbolic"
                            visible: root.isSelf
                            text: i18nc("@action:inmenu", "Draft Posts")
                            onTriggered: {
                                const page = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "ScheduledPostsPage"), { drafts: true });
                                page.opened.connect(function(id) {
                                    const composer = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                                        purpose: StatusComposer.New,
                                    });
                                    composer.openDraft(id);
                                });
                            }
                        },
                        Kirigami.Action {
                            icon.name: "open-link-symbolic"
                            text: i18nc("@action:inmenu 'Browser' being a web browser", "Open in Browser")
                            onTriggered: Qt.openUrlExternally(root.identity.url)
                        },
                        Kirigami.Action {
                            icon.name: "edit-copy"
                            text: i18n("Copy Link")
                            onTriggered: {
                                clipboard.content = root.identity.url;
                                applicationWindow().showPassiveNotification(i18n("Profile link copied."));
                            }
                        },
                        ShareAction {
                            id: shareAction

                            inputData: {
                                'urls': [root.identity.url.toString()],
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
            id: bioCard

            visible: root.identity.bio.length > 0

            FormCard.AbstractFormDelegate {
                background: null
                contentItem: QQC2.TextArea {
                    text: root.identity.bio
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
                    wrapMode: TextEdit.Wrap
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

        FormCard.FormCard {
            id: usernameCard

            Layout.topMargin: Kirigami.Units.largeSpacing

            FormCard.AbstractFormDelegate {
                topPadding: 0
                bottomPadding: 0
                hoverEnabled: false

                contentItem: RowLayout {
                    spacing: 0

                    QQC2.Label {
                        text: i18nc("@info:label Joined at", "Joined")
                        wrapMode: Text.Wrap

                        topPadding: Kirigami.Units.smallSpacing
                        bottomPadding: Kirigami.Units.smallSpacing

                        Layout.minimumWidth: Kirigami.Units.gridUnit * 7
                        Layout.maximumWidth: Kirigami.Units.gridUnit * 7
                    }

                    QQC2.TextArea {
                        Layout.fillWidth: true
                        readOnly: true
                        background: null
                        wrapMode: TextEdit.Wrap
                        textFormat: TextEdit.PlainText
                        text: root.identity.createdAt
                    }
                }
            }

            Repeater {
                model: root.identity.fields
                ColumnLayout {
                    id: delegate

                    required property var modelData

                    Layout.fillWidth: true
                    spacing: 0
                    FormCard.FormDelegateSeparator {
                        // We want it to always be visible regardless of hover state
                        opacity: 0.5
                    }

                    FormCard.AbstractFormDelegate {
                        topPadding: 0
                        bottomPadding: 0
                        hoverEnabled: true

                        QQC2.ToolTip.text: delegate.modelData.verified_at !== null ? i18n("Ownership of this link was checked on %1", Qt.formatDate(delegate.modelData.verified_at)) : ""
                        QQC2.ToolTip.visible: hovered && delegate.modelData.verified_at !== null
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                        background: Rectangle {
                            color: delegate.modelData.verified_at !== null ? Kirigami.Theme.positiveBackgroundColor : "transparent"
                        }

                        contentItem: RowLayout {
                            spacing: 0

                            Row {
                                spacing: Kirigami.Units.smallSpacing

                                Layout.minimumWidth: Kirigami.Units.gridUnit * 7
                                Layout.maximumWidth: Kirigami.Units.gridUnit * 7

                                QQC2.Label {
                                    text: delegate.modelData.name
                                    wrapMode: Text.Wrap

                                    topPadding: Kirigami.Units.smallSpacing
                                    bottomPadding: Kirigami.Units.smallSpacing
                                }

                                Kirigami.Icon {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    source: "checkmark-symbolic"
                                    width: Kirigami.Units.iconSizes.sizeForLabels
                                    height: Kirigami.Units.iconSizes.sizeForLabels
                                    visible: delegate.modelData.verified_at !== null
                                }
                            }

                            QQC2.TextArea {
                                Layout.fillWidth: true
                                readOnly: true
                                background: null
                                wrapMode: TextEdit.Wrap
                                textFormat: TextEdit.RichText
                                text: delegate.modelData.value
                                onLinkActivated: link => Qt.openUrlExternally(link)
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
            id: privateCard

            visible: root.identity.relationship

            Layout.topMargin: Kirigami.Units.largeSpacing

            FormCard.AbstractFormDelegate {
                background: null

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

                    RowLayout {
                        spacing: Kirigami.Units.mediumSpacing

                        Layout.fillWidth: true

                        Kirigami.Heading {
                            Layout.fillWidth: true
                            text: i18nc("@label:textfield", "Private Note:")
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
                        placeholderText: i18n("Write down something about this user, this isn't visible to anyone but you.")
                        textFormat: TextEdit.PlainText
                        wrapMode: TextEdit.Wrap
                        leftPadding: 0
                        rightPadding: 0
                        text: root.identity.relationship ? root.identity.relationship.note : ''
                        property string lastSavedText: ''
                        onActiveFocusChanged: {
                            lastSavedText = text;
                            if (activeFocus) {
                                autoSaveTimer.start()
                            } else {
                                AccountManager.selectedAccount.addNote(root.identity, text);
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
                                AccountManager.selectedAccount.addNote(root.identity, noteField.text);
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

                        Keys.onTabPressed: (event)=> {
                            nextItemInFocusChain(true).forceActiveFocus(Qt.TabFocusReason)
                            event.accepted = true
                        }
                    }
                }
            }
        }

        FormCard.FormCard {
            id: followingCard

            visible: followsRepeater.count !== 0

            Layout.topMargin: Kirigami.Units.largeSpacing

            FormCard.AbstractFormDelegate {
                background: null

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

                    QQC2.Label {
                        text: i18n("Also followed by:")
                    }
                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Repeater {
                            id: followsRepeater

                            model: LimiterModel {
                                id: limiterModel
                                maximumCount: 5
                                sourceModel: SocialGraphModel {
                                    name: "familiar_followers"
                                    accountId: root.identity.id
                                }
                            }

                            delegate: Components.AvatarButton {
                                required property var identity

                                source: identity.avatarUrl
                                cache: true
                                name: identity.displayName
                                Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                                onClicked: Navigation.openAccount(identity.id)
                            }
                        }
                        QQC2.Button {
                            text: limiterModel.extraCount > 0 ? i18nc("@action:button See all familiar followers", "View %1 More", limiterModel.extraCount) : i18nc("@action:button See all familiar followers", "View All")
                            onClicked: {
                                pageStack.push(socialGraphComponent, {
                                    name: "familiar_followers",
                                    accountId: root.identity.id
                                });
                            }
                        }
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

                spacing: Kirigami.Units.mediumSpacing

                readonly property FormCard.FormCard cardParent: privateCard.visible ? privateCard : bioCard

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    leftMargin: cardParent.cardWidthRestricted ? Math.round((cardParent.width - cardParent.maximumWidth) / 2) : Kirigami.Units.largeSpacing
                    rightMargin: cardParent.cardWidthRestricted ? Math.round((cardParent.width - cardParent.maximumWidth) / 2) : Kirigami.Units.largeSpacing
                }

                Kirigami.Chip {
                    closable: false
                    enabled: false

                    text: i18ncp("@label User's number of statuses", "<b>%1</b> post", "<b>%1</b> posts", root.identity.statusesCount)
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
                    text: i18ncp("@label User's number of followed accounts", "<b>%1</b> follows", "<b>%1</b> following", root.identity.followingCount)
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
                    text: i18ncp("@label User's number of followers", "<b>%1</b> follower", "<b>%1</b> followers", root.identity.followersCount)
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        QQC2.TabBar {
            id: bar

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View

            // Hack to disable the qqc2-desktop-style scrolling behavior.
            // This bar is on a scrollable page, you will eventually run into this tab bar which is annoying.
            background: null

            enabled: !root.loading

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true

            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Posts")
                implicitWidth: bar.parent.width / 3
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Posts && Replies")
                implicitWidth: bar.parent.width / 3
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Media")
                implicitWidth: bar.parent.width / 3
            }
        }
        Rectangle {
            Layout.fillWidth: true

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View

            enabled: !root.loading

            implicitHeight: extraLayout.implicitHeight + Kirigami.Units.largeSpacing * 2

            color: Kirigami.Theme.backgroundColor

            RowLayout {
                id: extraLayout

                spacing: Kirigami.Units.mediumSpacing

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
                    enabled: accountInfo.canExcludeBoosts && !root.loading

                    onToggled: accountInfo.excludeBoosts = checked
                }

                QQC2.ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    RowLayout {
                        spacing: Kirigami.Units.mediumSpacing

                        QQC2.ButtonGroup {
                            id: tagGroup
                        }

                        Kirigami.Chip {
                            text: i18nc("@action:button Show all of a profile's posts", "All")
                            closable: false
                            checked: true

                            onClicked: accountInfo.selectedTag = ""

                            QQC2.ButtonGroup.group: tagGroup
                        }

                        Repeater {
                            model: FeaturedTagsModel {
                                accountId: accountInfo.accountId
                            }

                            delegate: Kirigami.Chip {
                                required property string name

                                text: '#' + name
                                closable: false

                                onClicked: accountInfo.selectedTag = name

                                QQC2.ButtonGroup.group: tagGroup
                            }
                        }
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }
}
