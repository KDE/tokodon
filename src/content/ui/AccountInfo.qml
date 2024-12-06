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

Kirigami.Page {
    id: accountInfo

    required property string accountId

    property var postsBar
    property string selectedTag

    property var currentIndex: 0
    readonly property bool onPostsTab: currentIndex === 0
    readonly property bool onRepliesTab: currentIndex === 1
    readonly property bool onMediaTab: currentIndex === 2

    readonly property bool canExcludeBoosts: onPostsTab || onRepliesTab
    property alias excludeBoosts: accountModel.excludeBoosts

    readonly property bool largeScreen: width > Kirigami.Units.gridUnit * 25

    title: accountModel.identity.displayName
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight

        text: accountModel.identity.displayNameHtml

        textFormat: Text.StyledText
    }

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        color: Kirigami.Theme.backgroundColor
    }

    Connections {
        target: accountInfo.postsBar
        enabled: accountInfo.postsBar !== null
        ignoreUnknownSignals: true // postsBar is null when this is initially constructed

        function onCurrentIndexChanged(): void {
            accountInfo.currentIndex = accountInfo.postsBar.currentIndex;
        }
    }

    function updateTabs(): void {
        stackLayout.children[stackLayout.currentIndex].contentItem.headerItem.activateBar();
        postsBar.currentIndex = accountInfo.currentIndex;
    }

    StackLayout {
        id: stackLayout

        currentIndex: accountInfo.currentIndex === 2 ? 1 : 0

        anchors.fill: parent

        implicitHeight: children[currentIndex].implicitHeight

        onCurrentIndexChanged: accountInfo.updateTabs()

        Component.onCompleted: accountInfo.updateTabs()

        QQC2.ScrollView {
            focus: true
            clip: true

            Keys.onPressed: event => timelineView.handleKeyEvent(event)

            TimelineView {
                id: timelineView

                Kirigami.Theme.colorSet: Kirigami.Theme.View

                header: AccountHeader {}

                model: AccountModel {
                    id: accountModel

                    accountId: accountInfo.accountId
                    tagged: accountInfo.selectedTag
                    currentTab: accountInfo.currentIndex
                }

                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    text: i18nc("@label", "No Posts")
                    explanation: i18n("This user hasn't posted anything yet.")
                    visible: timelineView.count === 0 && !timelineView.model.loading
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }

        QQC2.ScrollView {
            clip: true

            GridView {
                id: gridView

                property int numCells: gridView.width < 1000 ? 3 : 5
                property real cellSize: gridView.width / numCells

                header: AccountHeader {}
                cellWidth: cellSize
                cellHeight: cellSize

                model: AccountMediaTimelineModel {
                    accountId: accountInfo.accountId
                    tagged: accountInfo.selectedTag
                }

                delegate: Item {
                    id: imageDelegate

                    required property string postId
                    required property string source
                    required property url tempSource
                    required property real focusX
                    required property real focusY
                    required property bool sensitive
                    required property var attachment

                    width: gridView.cellWidth
                    height: gridView.cellHeight

                    FocusedImage {
                        id: image

                        anchors.fill: parent

                        source: imageDelegate.source
                        focusX: imageDelegate.focusX
                        focusY: imageDelegate.focusY
                    }

                    Image {
                        id: tempImage

                        anchors.fill: parent

                        source: imageDelegate.tempSource
                        visible: image.status !== Image.Ready || imageDelegate.sensitive

                        Kirigami.Icon {
                            anchors.centerIn: parent
                            source: "view-hidden-symbolic"
                        }
                    }

                    Rectangle {
                        anchors.fill: parent

                        visible: hoverHandler.hovered
                        color: "transparent"

                        border {
                            width: 2
                            color: Kirigami.Theme.hoverColor
                        }
                    }

                    TapHandler {
                        onTapped: Navigation.openFullScreenImage([imageDelegate.attachment], accountModel.identity, 0);
                    }

                    HoverHandler {
                        id: hoverHandler
                        acceptedDevices: PointerDevice.AllDevices
                    }
                }

                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    text: i18nc("@label", "No Media")
                    explanation: i18n("This user hasn't posted any media yet.")
                    visible: gridView.count === 0 && !gridView.model.loading
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }
    }

    component AccountHeader: QQC2.Pane {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        width: parent.width

        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        topPadding: 0

        function activateBar(): void {
            accountInfo.postsBar = contentItem.item?.bar;
        }

        contentItem: ColumnLayout {
            id: layout

            spacing: 0

            property var bar

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

                            source: accountModel.identity.backgroundUrl
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
                        visible: accountModel.identity.relationship && accountModel.identity.relationship.followedBy

                        contentItem: QQC2.Label {
                            text: accountModel.identity.relationship && accountModel.identity.relationship.following && accountModel.identity.relationship.followedBy ? i18nc("'Mutual' is the common English word used in social media platforms describe people who follow you back. This the same meaning as 'mutual friends', for example.", "Mutual") : i18n("Follows you")
                            color: '#fafafa'
                        }

                        background: Rectangle {
                            radius: 3
                            color: '#090b0d'
                        }
                    }

                    QQC2.Control {
                        visible: accountModel.identity.bot

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

                                name: accountModel.identity.displayName
                                source: accountModel.identity.avatarUrl
                                imageMode: Components.Avatar.ImageMode.AdaptiveImageOrInitals

                                property Labs.AlbumModelItem album: Labs.AlbumModelItem {
                                    type: Labs.AlbumModelItem.Image
                                    source: accountModel.identity.avatarUrl
                                }

                                onClicked: Navigation.openFullScreenImage([album], accountModel.identity, 0);
                            }
                        }

                        ColumnLayout {
                            spacing: Kirigami.Units.smallSpacing

                            Layout.leftMargin: Kirigami.Units.largeSpacing
                            Layout.rightMargin: Kirigami.Units.largeSpacing
                            Layout.fillWidth: true

                            QQC2.Label {
                                Layout.fillWidth: true
                                text: accountModel.identity.displayNameHtml
                                font.bold: true
                                font.pixelSize: 24
                                maximumLineCount: 2
                                wrapMode: Text.Wrap
                                elide: Text.ElideRight
                            }

                            QQC2.TextArea {
                                text: "@" + accountModel.identity.account
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

                            Kirigami.ActionToolBar {
                                id: toolbar

                                flat: false
                                alignment: Qt.AlignLeft

                                KQuickControlsAddons.Clipboard { id: clipboard }

                                actions: [
                                    Kirigami.Action {
                                        icon.name: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.following) {
                                                return "list-remove-user";
                                            }
                                            return "list-add-user";
                                        }

                                        text: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.requested) {
                                                return i18n("Follow Requested");
                                            }
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.following) {
                                                return i18n("Unfollow");
                                            }
                                            return i18n("Follow");
                                        }
                                        onTriggered: {
                                            if (accountModel.identity.relationship.requested
                                                || accountModel.identity.relationship.following) {
                                                accountModel.account.unfollowAccount(accountModel.identity);
                                            } else {
                                                accountModel.account.followAccount(accountModel.identity);
                                            }
                                        }
                                        visible: !accountModel.isSelf
                                    },
                                    Kirigami.Action {
                                        displayHint: Kirigami.DisplayHint.IconOnly
                                        icon.name: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.notifying) {
                                                return "notifications-disabled";
                                            } else {
                                                return "notifications";
                                            }
                                        }

                                        visible: accountModel.identity.relationship && accountModel.identity.relationship.following && !accountModel.isSelf
                                        tooltip: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.notifying) {
                                                return i18n("Stop notifying me when %1 posts.", '@' + accountModel.identity.account);
                                            } else {
                                                return i18n("Notify me when %1 posts.", '@' + accountModel.identity.account);
                                            }
                                        }
                                        onTriggered: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.notifying) {
                                                accountModel.account.followAccount(accountModel.identity, accountModel.identity.relationship.showingReblogs, false);
                                            } else {
                                                accountModel.account.followAccount(accountModel.identity, accountModel.identity.relationship.showingReblogs, true);
                                            }
                                        }
                                    },
                                    Kirigami.Action {
                                        icon.name: "list-add"
                                        visible: !accountModel.isSelf
                                        text: i18n("Mention…")
                                        onTriggered: Navigation.openComposer("@" + accountModel.identity.account + " ")
                                    },
                                    Kirigami.Action {
                                        icon.name: "tokodon-chat-reply"
                                        visible: !accountModel.isSelf
                                        text: i18n("Start a Conversation…")
                                        onTriggered: Navigation.openConversation(accountModel.identity.account)
                                    },
                                    Kirigami.Action {
                                        icon.name: "view-hidden"
                                        displayHint: Kirigami.DisplayHint.IconOnly
                                        visible: accountModel.identity.relationship && accountModel.identity.relationship.following && !accountModel.isSelf
                                        text: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.showingReblogs) {
                                                return i18n("Hide Boosts from %1", '@' + accountModel.identity.account);
                                            } else {
                                                return i18n("Show Boosts from %1", '@' + accountModel.identity.account);
                                            }
                                        }
                                        onTriggered: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.showingReblogs) {
                                                accountModel.account.followAccount(accountModel.identity, false, accountModel.identity.relationship.notifying);
                                            } else {
                                                accountModel.account.followAccount(accountModel.identity, true, accountModel.identity.relationship.notifying);
                                            }
                                        }
                                    },
                                    Kirigami.Action {
                                        icon.name: "favorite"
                                        visible: accountModel.identity.relationship && !accountModel.isSelf
                                        text: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.endorsed) {
                                                return i18n("Stop Featuring This Profile");
                                            } else {
                                                return i18n("Feature This Profile");
                                            }
                                        }
                                        onTriggered: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.endorsed) {
                                                accountModel.account.unpin(accountModel.identity);
                                            } else {
                                                accountModel.account.pin(accountModel.identity);
                                            }
                                        }
                                    },
                                    Kirigami.Action {
                                        icon.name: "dialog-cancel"
                                        visible: accountModel.identity.relationship && !accountModel.isSelf
                                        text: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.muting) {
                                                return i18n("Unmute");
                                            } else {
                                                return i18n("Mute");
                                            }
                                        }
                                        onTriggered: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.muting) {
                                                accountModel.account.unmuteAccount(accountModel.identity);
                                            } else {
                                                accountModel.account.muteAccount(accountModel.identity);
                                            }
                                        }
                                    },
                                    Kirigami.Action {
                                        icon.name: "im-ban-kick-user"
                                        visible: accountModel.identity.relationship && !accountModel.isSelf
                                        text: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.blocking) {
                                                return i18n("Unblock");
                                            } else {
                                                return i18n("Block");
                                            }
                                        }
                                        onTriggered: {
                                            if (accountModel.identity.relationship && accountModel.identity.relationship.blocking) {
                                                accountModel.account.unblock(accountModel.identity);
                                            } else {
                                                accountModel.account.block(accountModel.identity);
                                            }
                                        }
                                    },
                                    Kirigami.Action {
                                        icon.name: "dialog-warning-symbolic"
                                        visible: !accountModel.isSelf
                                        text: i18nc("@action:inmenu Report this post", "Report…");
                                        onTriggered: Navigation.reportUser(accountModel.identity)
                                    },
                                    Kirigami.Action {
                                        icon.name: "user-group-properties"
                                        visible: accountModel.isSelf
                                        text: i18n("Edit Profile")
                                        onTriggered: pageStack.push(Qt.createComponent("org.kde.tokodon", "EditProfilePage"), {
                                                            account: accountModel.account
                                                        }, {
                                                            title: i18n("Account editor")
                                                        })
                                    },
                                    Kirigami.Action {
                                        visible: accountModel.isSelf
                                        fromQAction: (toolbar.QQC2.ApplicationWindow.window as StatefulApp.StatefulWindow)?.application.action('options_configure') ?? null
                                    },
                                    Kirigami.Action {
                                        icon.name: "favorite"
                                        visible: accountModel.isSelf
                                        text: i18n("Featured Users")
                                        onTriggered: pageStack.push(socialGraphComponent, { name: "featured" });
                                    },
                                    Kirigami.Action {
                                        icon.name: "window"
                                        text: i18nc("@action:inmenu 'Browser' being a web browser", "Open in Browser")
                                        onTriggered: Qt.openUrlExternally(accountModel.identity.url)
                                    },
                                    Kirigami.Action {
                                        icon.name: "edit-copy"
                                        text: i18n("Copy Link")
                                        onTriggered: {
                                            clipboard.content = accountModel.identity.url;
                                            applicationWindow().showPassiveNotification(i18n("Profile link copied."));
                                        }
                                    },
                                    ShareAction {
                                        id: shareAction

                                        inputData: {
                                            'urls': [accountModel.identity.url.toString()],
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

                visible: accountModel.identity.bio.length > 0

                FormCard.AbstractFormDelegate {
                    background: null
                    contentItem: QQC2.TextArea {
                        text: accountModel.identity.bio
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
                            text: accountModel.identity.createdAt
                        }
                    }
                }

                Repeater {
                    model: accountModel.identity.fields
                    ColumnLayout {
                        id: delegate

                        required property var modelData

                        Layout.fillWidth: true
                        spacing: 0
                        FormCard.FormDelegateSeparator {}

                        FormCard.AbstractFormDelegate {
                            topPadding: 0
                            bottomPadding: 0
                            hoverEnabled: false

                            background: Rectangle {
                                color: delegate.modelData.verified_at !== null ? Kirigami.Theme.positiveBackgroundColor : "transparent"
                            }

                            contentItem: RowLayout {
                                spacing: 0

                                QQC2.Label {
                                    text: delegate.modelData.name
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

                visible: accountModel.identity.relationship

                Layout.topMargin: Kirigami.Units.largeSpacing

                FormCard.AbstractFormDelegate {
                    background: null

                    contentItem: ColumnLayout {
                        spacing: 0
                        RowLayout {
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
                            text: accountModel.identity.relationship ? accountModel.identity.relationship.note : ''
                            property string lastSavedText: ''
                            onActiveFocusChanged: {
                                lastSavedText = text;
                                if (activeFocus) {
                                    autoSaveTimer.start()
                                } else {
                                    accountModel.account.addNote(accountModel.identity, text);
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
                                    accountModel.account.addNote(accountModel.identity, noteField.text);
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
                    QQC2.Label {
                        text: i18n("Also followed by:")
                    }
                    RowLayout {
                        Repeater {
                            id: followsRepeater

                            model: LimiterModel {
                                id: limiterModel
                                maximumCount: 5
                                sourceModel: SocialGraphModel {
                                    name: "familiar_followers"
                                    accountId: accountModel.identity.id
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
                                    accountId: accountModel.identity.id
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

                        text: i18ncp("@label User's number of statuses", "<b>%1</b> post", "<b>%1</b> posts", accountModel.identity.statusesCount)
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
                        text: i18ncp("@label User's number of followed accounts", "<b>%1</b> follows", "<b>%1</b> following", accountModel.identity.followingCount)
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
                        text: i18ncp("@label User's number of followers", "<b>%1</b> follower", "<b>%1</b> followers", accountModel.identity.followersCount)
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

                Component.onCompleted: layout.bar = bar

                enabled: !accountModel.loading

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

                enabled: !accountModel.loading

                implicitHeight: extraLayout.implicitHeight + Kirigami.Units.largeSpacing * 2

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
                        enabled: accountInfo.canExcludeBoosts && !accountModel.loading

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
}
