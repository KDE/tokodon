// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon
import org.kde.tokodon.private
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import org.kde.config as KConfig

import "./StatusComposer"
import "./PostDelegate"

StatefulApp.StatefulWindow {
    id: root

    application: TokodonApplication {
        accountManager: AccountManager

        configurationView: TokodonConfigurationView {
            window: root
            application: root.application
        }
    }

    title: pageStack.currentItem?.title ?? ""
    windowName: "Main"

    property bool isShowingFullScreenImage: false

    // A new post was created by us. Currently used by ThreadPages to update themselves when we reply.
    signal newPost()

    minimumWidth: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 22
    minimumHeight: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 20

    pageStack {
        defaultColumnWidth: root.width

        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (root.pageStack.currentIndex > 0
                || root.pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }
    }

    function decideDefaultPage(): void {
        pageStack.clear();

        if (InitialSetupFlow.isSetupNeeded()) {
            globalDrawer.drawerOpen = false;
            pageStack.push(Qt.createComponent("org.kde.tokodon", "SetupWelcome"));
            return;
        }

        if (AccountManager.selectedAccountHasIssue) {
            pageStack.push(Qt.createComponent("org.kde.tokodon", "LoginIssuePage"));
        } else {
            homeAction.trigger();
        }
    }

    function startupAccountCheck(): void {
        if (AccountManager.hasAccounts) {
            decideDefaultPage();
        } else {
            pageStack.push(Qt.createComponent("org.kde.tokodon", "WelcomePage"));
        }
    }

    function navigateLink(link: string, shouldOpenInternalLinks: bool): void {
        if (link.startsWith('hashtag:/') && shouldOpenInternalLinks) {
            pageStack.push(tagModelComponent, {
                hashtag: link.substring(9),
            })
        } else if (link.startsWith('account:/') && shouldOpenInternalLinks) {
            Navigation.openAccount(link.substring(9))
        } else if (link.startsWith('web+ap:/') && shouldOpenInternalLinks) {
            Controller.openWebApLink(link.substring(8))
        } else {
            Controller.openLink(link);
        }
    }

    function requestCrossAction(action: string, url: string): void {
        crossActionDialog.action = action;
        crossActionDialog.url = url;
        crossActionDialog.open();
    }

    Kirigami.Action {
        fromQAction: root.application.action('open_status_composer')
    }

    Kirigami.Action {
        fromQAction: root.application.action('open_kcommand_bar')
    }

    ReportDialog {
        id: reportDialog
        visible: false
    }

    Kirigami.PromptDialog {
        id: crossActionDialog

        property string url
        property string action

        title: {
            if (action === 'open') {
                return i18nc("@title", "Open As…")
            } else if (action === 'reply') {
                return i18nc("@title", "Reply As…")
            } else if (action === 'favourite') {
                return i18nc("@title", "Favorite As…")
            } else if (action === 'reblog') {
                return i18nc("@title", "Boost As…")
            } else if (action === 'bookmark') {
                return i18nc("@title", "Bookmark As…")
            } else {
                return i18nc("@title", "Unknown Action")
            }
        }

        standardButtons: Kirigami.Dialog.NoButton

        mainItem: ColumnLayout {
            Repeater {
                id: accounts

                model: AccountManager

                delegate: Delegates.RoundedItemDelegate {
                    required property int index
                    required property string displayName
                    required property string instance
                    required property var account

                    text: displayName

                    Layout.fillWidth: true

                    onClicked: crossActionDialog.takeAction(account)
                }
            }
        }

        function takeAction(account: AbstractAccount): void {
            if (action === 'open' || action === 'reply') {
                AccountManager.selectedAccount = account;
            }

            Qt.callLater(() => {
                if (action === 'open') {
                    Controller.openWebApLink(url);
                } else {
                    account.mutateRemotePost(url, action);
                }
            });

            close();
        }
    }

    Component.onCompleted: {
        if (AccountManager.isReady) {
            startupAccountCheck();
        }

        homeAction.checked = true;
    }

    Connections {
        target: AccountManager

        function onAccountSelected(): void {
            decideDefaultPage();
        }

        function onAccountRemoved(): void {
            if (!AccountManager.hasAccounts) {
                pageStack.replace(Qt.createComponent("org.kde.tokodon", "WelcomePage"));
                globalDrawer.drawerOpen = false;
            }
        }

        function onAccountsReloaded(): void {
            pageStack.replace(mainTimeline.createObject(root), {
                name: "home"
            });
        }

        function onAccountsReady(): void {
            root.startupAccountCheck();
        }
    }

    Connections {
        target: root.application

        function onConfigureAccount(account: AbstractAccount): void {
            root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "ProfileEditor"), {
                account: account,
            });
        }

        function onAddAccount(): void {
            let page = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "WelcomePage"));
            page.QQC2.ApplicationWindow.window.pageStack.columnView.columnResizeMode = Kirigami.ColumnView.SingleColumn;
        }
    }

    function popoutStatusComposer(originalEditor: StatusComposer): void {
        const item = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
            closeApplicationWhenFinished: true,
            purpose: originalEditor.purpose,
            initialText: originalEditor.backend.status,
            inReplyTo: originalEditor.inReplyTo,
            previewPost: originalEditor.previewPost
        }, {
            title: i18n("Write a new post"),
            width: Kirigami.Units.gridUnit * 30,
            height: Kirigami.Units.gridUnit * 30,
            modality: Qt.NonModal
        });
        item.backend.copyFromOther(originalEditor.backend);
        item.refreshData();
        item.Window.window.closing.connect(event => {
            if (item.shouldClose()) {
                event.accepted = false;
            }
        });
    }

    Component {
        id: fullScreenImage
        FullScreenImage {}
    }

    Connections {
        target: Navigation

        function onOpenFullScreenImage(attachments: var, identity: Identity, currentIndex: int): void {
            const dialog = fullScreenImage.createObject(parent, {
                attachments: attachments,
                identity: identity,
                initialIndex: currentIndex,
            });
            dialog.open();
        }

        function onOpenComposer(text: string): void {
            if (Config.popOutByDefault) {
                const item = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                    closeApplicationWhenFinished: true,
                    purpose: StatusComposer.New,
                    initialText: text
                }, {
                    title: i18n("Write a new post"),
                    width: Kirigami.Units.gridUnit * 30,
                    height: Kirigami.Units.gridUnit * 30,
                    modality: Qt.NonModal
                });
                item.Window.window.closing.connect(event => {
                    if (item.shouldClose()) {
                        event.accepted = false;
                    }
                });
            } else {
                pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                    purpose: StatusComposer.New,
                    initialText: text
                });
            }
        }

        function onReplyTo(post: Post): void {
            if (Config.popOutByDefault) {
                const item = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                    closeApplicationWhenFinished: true,
                    purpose: StatusComposer.Reply,
                }, {
                    title: i18n("Reply to post"),
                    width: Kirigami.Units.gridUnit * 30,
                    height: Kirigami.Units.gridUnit * 30,
                    modality: Qt.NonModal
                });
                item.backend.setupReplyTo(post);
                item.refreshData();
                item.Window.window.closing.connect(event => {
                    if (item.shouldClose()) {
                        event.accepted = false;
                    }
                });
            } else {
                const item = pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                    purpose: StatusComposer.Reply,
                    previewPost: post
                });
                item.backend.setupReplyTo(post);
                item.refreshData();
            }
        }

        function onOpenPost(postId: string): void {
            if (!pageStack.currentItem.postId || pageStack.currentItem.postId !== postId) {
                pageStack.push(Qt.createComponent("org.kde.tokodon", "ThreadPage"), {
                    postId: postId,
                });
            }
        }

        function onOpenAccount(accountId: string): void {
            if (!pageStack.currentItem.accountId || pageStack.currentItem.accountId !== accountId) {
                pageStack.push(Qt.createComponent("org.kde.tokodon", "AccountInfo"), {
                    accountId: accountId,
                });
            }
        }

        function onOpenTag(tag: string): void {
            pageStack.push(tagModelComponent, {
                hashtag: tag,
            })
        }

        function onReportPost(identity: var, postId: string): void {
            root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "ReportDialog"),
                {
                    type: ReportDialog.Post,
                    identity: identity,
                    postId: postId
                });
        }

        function onReportUser(identity: Identity): void {
            root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "ReportDialog"),
                {
                    type: ReportDialog.User,
                    identity: identity
                });
        }

        function onOpenList(listId: string, name: string): void {
            pageStack.push(listTimelinePage.createObject(root, {
                name,
                listId
            }));
        }
    }

    globalDrawer: Sidebar {
        id: drawer

        application: root.application
        shouldCollapse: root.width < Kirigami.Units.gridUnit * 50
        actions: Kirigami.Settings.isMobile ?
            [searchAction, announcementsAction, followRequestAction, followingAction, exploreAction, conversationAction, favouritesAction, bookmarksAction, listsAction] :
            [homeAction, notificationAction, searchAction, announcementsAction, followRequestAction, followingAction, localTimelineAction, globalTimelineAction, exploreAction, conversationAction, favouritesAction, bookmarksAction, listsAction]
        bottomActions: [debugAction, moderationToolsAction, configureAction]
    }

    property Kirigami.Action homeAction: Kirigami.Action {
        icon.name: "go-home-large"
        text: i18n("Home")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline.createObject(root), {
                name: "home",
                iconName: "go-home-large",
                placeholderText: i18n("No Posts"),
                placeholderExplanation: i18n("It seems pretty quiet right now, try posting something!")
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action notificationAction: Kirigami.Action {
        readonly property int alertCount: AccountManager.selectedAccount.unreadNotificationsCount

        icon.name: "notifications"
        text: i18n("Notifications")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(notificationTimeline.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action followRequestAction: Kirigami.Action {
        readonly property int alertCount: AccountManager.selectedAccount.followRequestCount

        icon.name: "list-add-user"
        text: i18n("Follow Requests")
        checkable: true
        visible: AccountManager.hasAccounts && AccountManager.selectedAccount && alertCount > 0
        onTriggered: {
            pageStack.clear();
            pageStack.push(socialGraphComponent.createObject(root), {
                name: "request",
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action localTimelineAction: Kirigami.Action {
        icon.name: "system-users"
        text: i18n("Local")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline.createObject(root), {
                name: "public",
                iconName: "system-users",
                placeholderText: i18n("No Posts"),
                placeholderExplanation: i18n("It seems pretty quiet right now, try posting something!")
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action globalTimelineAction: Kirigami.Action {
        icon.name: "kstars_xplanet"
        text: i18n("Global")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline.createObject(root), {
                name: "federated",
                iconName: "kstars_xplanet",
                placeholderText: i18n("No Posts"),
                placeholderExplanation: i18n("It seems pretty quiet right now, try posting something!")
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action conversationAction: Kirigami.Action {
        icon.name: "tokodon-chat-reply"
        text: i18n("Conversations")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(conversationPage.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action favouritesAction: Kirigami.Action {
        icon.name: "tokodon-post-favorite"
        text: i18n("Favorites")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline.createObject(root), {
                name: "favourites",
                iconName: "tokodon-post-favorite",
                placeholderText: i18n("No Favorites"),
                placeholderExplanation: i18n("Posts that you favorite will show up here. If you appreciate someone's post, favorite it!")
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action bookmarksAction: Kirigami.Action {
        icon.name: "bookmarks"
        text: i18n("Bookmarks")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline.createObject(root), {
                name: "bookmarks",
                iconName: "bookmarks",
                placeholderText: i18n("No Bookmarks"),
                placeholderExplanation: i18n("Bookmark posts and they will show up here. Bookmarks are always kept private, even to the post's author.")
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action exploreAction: Kirigami.Action {
        icon.name: "kstars_planets"
        text: i18n("Explore")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(exploreTimeline.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action followingAction: Kirigami.Action {
        icon.name: "user-group-properties-symbolic"
        text: i18n("Following")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(followingTimeline.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action searchAction: Kirigami.Action {
        icon.name: "search"
        text: i18n("Search")
        checkable: true
        visible: Kirigami.Settings.isMobile
        onTriggered: {
            pageStack.clear();
            pageStack.push(searchPage.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action announcementsAction: Kirigami.Action {
        icon.name: "note"
        text: i18nc("@action:button Server Announcements", "Announcements")
        checkable: true
        visible: AccountManager.hasAccounts && AccountManager.selectedAccount
        onTriggered: {
            pageStack.clear();
            pageStack.push(announcementsPage.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action listsAction: Kirigami.Action {
        icon.name: "view-list-text"
        text: i18n("Lists")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(listsPage.createObject(root));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action debugAction: Kirigami.Action {
        icon.name: "debug-run"
        onTriggered: {
            pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "DebugPage"))
            if (drawer.modal) {
                drawer.close();
            }
        }
        visible: AccountManager.testMode
        text: i18nc("@action:button Open debug page", "Debug")
    }

    property ModerationToolsView moderationToolsView: ModerationToolsView {
        id: moderationToolsView
        window: root
    }

    property Kirigami.Action moderationToolsAction: Kirigami.Action {
        icon.name: "lock"
        text: i18nc("@action:button Open moderation tools", "Moderation Tools")
        visible: AccountManager.selectedAccount && (AccountManager.selectedAccount.identity.permission & AdminAccountInfo.ManageUsers)

        onTriggered: {
            moderationToolsView.open()
            if (drawer.modal) {
                drawer.close();
            }
        }
    }

    property Kirigami.Action configureAction: Kirigami.Action {
        text: i18nc("@action:button Open settings dialog", "Settings")
        fromQAction: root.application.action('options_configure')
        onTriggered: if (drawer.modal) {
            drawer.close();
        }
    }

    property Component announcementsPage: Qt.createComponent("org.kde.tokodon", "AnnouncementsPage", Qt.Asynchronous)
    property Component listsPage: Qt.createComponent("org.kde.tokodon", "ListsPage", Qt.Asynchronous)
    property Component searchPage: Qt.createComponent("org.kde.tokodon", "SearchPage", Qt.Asynchronous)
    property Component conversationPage: Qt.createComponent("org.kde.tokodon", "ConversationPage", Qt.Asynchronous)
    property Component listTimelinePage: Qt.createComponent("org.kde.tokodon", "ListTimelinePage", Qt.Asynchronous)

    property Kirigami.NavigationTabBar tabBar: Kirigami.NavigationTabBar {
        // Make sure we take in count drawer width
        visible: pageStack.layers.depth <= 1 && AccountManager.hasAccounts && !root.wideScreen
        actions: [homeAction, notificationAction, localTimelineAction, globalTimelineAction]
        enabled: !AccountManager.selectedAccountHasIssue
    }

    footer: Kirigami.Settings.isMobile ? tabBar : null

    Component {
        id: mainTimeline
        TimelinePage {
            id: timelinePage

            property alias name: timelineModel.name

            model: MainTimelineModel {
                id: timelineModel
                showReplies: timelinePage.showReplies
                showBoosts: timelinePage.showBoosts
            }
        }
    }

    Component {
        id: socialGraphComponent
        SocialGraphPage {
            id: socialGraphPage
            property alias name: socialGraphModel.name
            property alias accountId: socialGraphModel.accountId
            property alias statusId: socialGraphModel.statusId
            property alias count: socialGraphModel.count
            model: SocialGraphModel {
                id: socialGraphModel
                name: socialGraphPage.name
                accountId: socialGraphPage.accountId
                statusId: socialGraphPage.statusId
                count: socialGraphPage.count
            }
        }
    }

    Component {
        id: notificationTimeline
        NotificationPage {}
    }

    Component {
        id: exploreTimeline
        ExplorePage {}
    }

    Component {
        id: followingTimeline
        FollowingPage {}
    }

    property Item hoverLinkIndicator: QQC2.Control {
        parent: overlay
        property alias text: linkText.text
        opacity: text.length > 0 ? 1 : 0
        visible: !Kirigami.Settings.isMobile && !text.startsWith("hashtag:") && !text.startsWith("account:")

        z: root.globalDrawer.z + 1
        x: 0
        y: parent.height - implicitHeight
        contentItem: QQC2.Label {
            id: linkText
        }
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        background: Rectangle {
             color: Kirigami.Theme.backgroundColor
        }
    }

    Component {
        id: tagModelComponent
        TimelinePage {
            id: tagPage
            property string hashtag
            model: TagsTimelineModel {
                hashtag: tagPage.hashtag
                showReplies: tagPage.showReplies
                showBoosts: tagPage.showBoosts
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: !AccountManager.isReady
        color: Kirigami.Theme.backgroundColor

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
        }
    }

    KConfig.WindowStateSaver {
        configGroupName: "MainWindow"
    }

    Connections {
        target: AccountManager.selectedAccount

        function onFetchedOEmbed(html: string): void {
            embedDialog.active = true;
            embedDialog.item.html = html;
            embedDialog.item.open()
        }
    }

    Loader {
        id: embedDialog

        active: false
        sourceComponent: Kirigami.PromptDialog {
            property alias html: textArea.text

            title: i18nc("@title", "Embed Information")
            mainItem: QQC2.TextArea {
                id: textArea

                readOnly: true
                wrapMode: TextEdit.Wrap

                Kirigami.SpellCheck.enabled: false
            }
            standardButtons: Kirigami.Dialog.Ok
            showCloseButton: false
        }
    }
}
