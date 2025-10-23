// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Templates as T
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon

import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.statefulapp as StatefulApp

import "./StatusComposer"
import "./PostDelegate"

StatefulApp.StatefulWindow {
    id: root

    property QtObject application: TokodonApplication {
        accountManager: AccountManager
        // FIXME: avoidable?
        function uncheckMainActions() {
            dummyAction.checked = true;
        }
    }

    TokodonConfigurationView {
        id: configurationView
        window: root
        application: root.application
    }

    property alias globalActions: globalActions
    property alias systemActions: systemActions

    ActionGroup {
        id: pagesGroup
        exclusionPolicy: ActionGroup.Exclusive
    }
    ActionCollection {
        id: globalActions
        name: "tokodon_actions"
        ActionData {
            name: "open_status_composer"
            icon.name: "list-add"
            text: i18nc("@action:button", "Write a New Post")
            defaultShortcut: "Ctrl+N"
            action: Kirigami.Action {
                onTriggered: root.openComposer("", undefined);
            }
        }
        ActionData {
            name: "add_account"
            icon.name: "list-add-user-symbolic"
            text: i18nc("@action:button", "Add Account")
            action: Kirigami.Action {
                onTriggered: {
                    let page;
                    // pushDialogLayer is inherently broken in Kirigami, so let's push it as a page on mobile instead
                    if (Kirigami.Settings.isMobile) {
                        page = root.pageStack.push(Qt.createComponent("org.kde.tokodon", "WelcomePage"), { application: root.application, showSettingsButton: false });
                    } else {
                        page = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "WelcomePage"), { application: root.application, showSettingsButton: false });
                    }
                    page.QQC2.ApplicationWindow.window.pageStack.columnView.columnResizeMode = Kirigami.ColumnView.SingleColumn;
                }
            }
        }
        ActionData {
            name: "home_timeline"
            icon.name: "go-home-large"
            text: i18nc("@action:button Home Timeline", "Home")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: homeAction
                onTriggered: {
                    if (root.checkIfCurrentPage("home")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(mainTimeline.createObject(root), {
                        pageId: "home",
                        name: "home",
                        iconName: "go-home-large",
                        placeholderText: i18n("No Posts"),
                        placeholderExplanation: i18n("It seems pretty quiet right now, try posting something!")
                    });
                }
            }
        }
        ActionData {
            name: "notifications"
            icon.name: "notifications"
            text: i18nc("@action:button Account Notifications", "Notifications")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: notificationAction
                onTriggered: {
                    if (root.checkIfCurrentPage("notifications")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(notificationTimeline.createObject(root, { pageId: "notifications" }));
                }
            }
        }
        ActionData {
            name: "follow_requests"
            icon.name: "list-add-user"
            text: i18nc("@action:button Follows that require explicit allow/deny", "Follow Requests")
            actionGroup: pagesGroup
            checkable: true
            visible: AccountManager.hasAccounts && AccountManager.selectedAccount && alertCount > 0
            action: Kirigami.Action {
                id: followRequestAction
                onTriggered: {
                    if (root.checkIfCurrentPage("followRequests")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(socialGraphComponent.createObject(root), {
                        pageId: "followRequests",
                        name: "request",
                    });
                }
            }
        }
        ActionData { //TODO: actiongroup
            name: "local_timeline"
            icon.name: "system-users"
            text: i18nc("@action:button Local timeline of posts from the account's own server", "Local")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: localTimelineAction
                onTriggered: {
                    if (root.checkIfCurrentPage("local")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(mainTimeline.createObject(root), {
                        pageId: "local",
                        name: "public",
                        iconName: "system-users",
                        placeholderText: i18n("No Posts"),
                        placeholderExplanation: i18n("It seems pretty quiet right now, try posting something!")
                    });
                }
            }
        }
        ActionData {
            name: "global_timeline"
            icon.name: "kstars_xplanet"
            text: i18nc("@action:button Global timeline of posts from the entire Fediverse network", "Open Global Timeline")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: globalTimelineAction
                onTriggered: {
                    if (root.checkIfCurrentPage("global")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(mainTimeline.createObject(root), {
                        pageId: "global",
                        name: "federated",
                        iconName: "kstars_xplanet",
                        placeholderText: i18n("No Posts"),
                        placeholderExplanation: i18n("It seems pretty quiet right now, try posting something!")
                    });
                }
            }
        }
        ActionData {
            name: "conversations"
            icon.name: "view-conversation-balloon-symbolic"
            text: i18nc("@action:button Direct one-on-one messages between users", "Conversations")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: conversationAction
                onTriggered: {
                    if (root.checkIfCurrentPage("conversations")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(Qt.createComponent("org.kde.tokodon", "ConversationPage"), {
                        pageId: "conversations",
                    });
                }
            }
        }
        ActionData {
            name: "account_switcher"
            icon.name: "user"
            text: i18nc("@action:button", "Open Account Switcher")
            actionGroup: pagesGroup
            action: Kirigami.Action {
                id: accountSwitcherAction
                onTriggered: {
                    let dialog = Qt.createComponent("org.kde.tokodon", "AccountSwitchDialog").createObject(root.QQC2.Overlay.overlay, {
                        application: root.application,
                        userInfo: root,
                    });
                    dialog.open();
                }
            }
        }
        ActionData {
            name: "favorites"
            icon.name: "favorite"
            text: i18nc("@action:button This account's favorited posts", "Favorites")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: favouritesAction
                onTriggered: {
                    if (root.checkIfCurrentPage("favorites")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(mainTimeline.createObject(root), {
                        pageId: "favorites",
                        name: "favourites",
                        iconName: "favorite",
                        placeholderText: i18n("No Favorites"),
                        placeholderExplanation: i18n("Posts that you favorite will show up here. If you appreciate someone's post, favorite it!")
                    });
                }
            }
        }
        ActionData {
            name: "bookmarks"
            icon.name: "bookmarks"
            text: i18nc("@action:button This account's bookmarked posts", "Bookmarks")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: bookmarksAction
                onTriggered: {
                    if (root.checkIfCurrentPage("bookmarks")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(mainTimeline.createObject(root), {
                        pageId: "bookmarks",
                        name: "bookmarks",
                        iconName: "bookmarks",
                        placeholderText: i18n("No Bookmarks"),
                        placeholderExplanation: i18n("Bookmark posts and they will show up here. Bookmarks are always kept private, even to the post's author.")
                    });
                }
            }
        }
        ActionData {
            name: "explore"
            icon.name: "kstars_planets"
            text: i18nc("@action:button Explore this server's trending posts, news, and more", "Explore")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: exploreAction
                onTriggered: {
                    if (root.checkIfCurrentPage("explore")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(exploreTimeline.createObject(root, { pageId: "explore" }));
                }
            }
        }
        ActionData {
            name: "following"
            icon.name: "user-group-properties-symbolic"
            text: i18nc("@action:button A list of this account's followed accounts", "Following")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: followingAction
                onTriggered: {
                    if (root.checkIfCurrentPage("following")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(followingTimeline.createObject(root, { pageId: "following" }));
                }
            }
        }
        ActionData {
            name: "search"
            icon.name: "search"
            text: i18nc("@action:button Search for users, posts and tags", "Open Search")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: searchAction
                onTriggered: {
                    if (root.checkIfCurrentPage("search")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(searchPage.createObject(root, { pageId: "search" }));
                }
            }
        }
        ActionData {
            name: "server_information"
            icon.name: "note"
            text: i18nc("@action:button", "Open Server Information")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: serverInformationAction
                onTriggered:{
                    if (root.checkIfCurrentPage("serverInformation")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(serverInformationPage.createObject(root, { pageId: "serverInformation" }));
                }
            }
        }
        ActionData {
            name: "lists"
            icon.name: "view-list-text"
            text: i18nc("@action:button This account's lists, or timelines consisting of a groups of accounts", "Open Lists")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: listsAction
                onTriggered: {
                    if (root.checkIfCurrentPage("lists")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(listsPage.createObject(root, { pageId: "lists" }));
                }
            }
        }
        ActionData {
            name: "profile"
            icon.name: "user"
            text: i18nc("@action:button This account's profile", "Open Profile")
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {
                id: profileAction
                onTriggered: {
                    if (root.checkIfCurrentPage("profile")) {
                        return;
                    }

                    pageStack.clear();
                    pageStack.push(Qt.createComponent("org.kde.tokodon", "AccountInfo"), {
                        pageId: "profile",
                        accountId: AccountManager.selectedAccountId,
                    });
                }
            }
        }
        ActionData {
            // FIXME: avoidable?
            id: dummyAction
            name: "dummy_action"
            visible: false
            actionGroup: pagesGroup
            checkable: true
            action: Kirigami.Action {}
        }
    }

    ActionCollection {
        id: systemActions
        name: "system_actions"
        ActionData {
            name: "open_kcommand_bar"
            icon.name: "edit-find-symbolic"
            text: i18nc("@action:button Open Actions Command Bar", "Find Action...")
            defaultShortcut: "Ctrl+Alt+I"
            action: Kirigami.Action {
                onTriggered: commandBarPage.open()
            }
        }
        ActionData {
            name: "options_configure"
            icon.name: "settings-configure"
            text: i18nc("@action:button Open settings dialog", "Settings")
            action: Kirigami.Action {
                id: configureAction
                onTriggered: configurationView.open()
            }
        }
    }

    component AccountActionCollection: ActionCollection {
        id: collection
        name: accountString + "_actions"
        required property AbstractAccount account
        readonly property string accountString: account.username + (new URL(account.instanceUri)).host
        ActionData {
            name: "switch_account_" + accountString
            text: i18nc("@action:button", "Switch '%1'", account.username)
            action: Kirigami.Action {
                onTriggered: root.application.switchAccount(account)
            }
        }
        ActionData {
            name: "configure_account_" + collection.accountString
            text: i18nc("@action:button", "Configure '%1'", account.username)
            action: Kirigami.Action {
                onTriggered: {
                    root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "ProfileEditor"), {
                        account: account,
                    });
                }
            }
        }
    }

    Instantiator {
        model: AccountManager
        delegate: AccountActionCollection {}
    }

    KQuickCommandBarPage {
        id: commandBarPage
    }


    title: pageStack.currentItem?.title ?? ""
    windowName: "Main"

    property bool isShowingFullScreenImage: false
    readonly property bool wideMode: root.width >= Kirigami.Units.gridUnit * 50

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

    header: Kirigami.Separator {
        width: root.width
        height: visible ? 1 : 0 // HACK The height is used for the position of the sidebar. Remove when we depends on Kirigami 6.10
        visible: !AccountManager.isReady
    }

    function decideDefaultPage(): void {
        root.pageStack.clear();

        if (InitialSetupFlow.isSetupNeeded()) {
            globalDrawer.drawerOpen = false;
            root.pageStack.push(Qt.createComponent("org.kde.tokodon", "SetupWelcome"));
            return;
        }

        if (AccountManager.selectedAccountHasIssue) {
            root.pageStack.push(Qt.createComponent("org.kde.tokodon", "LoginIssuePage"));
        } else {
            homeAction.trigger();
        }
    }

    function startupAccountCheck(): void {
        if (AccountManager.hasAccounts) {
            decideDefaultPage();
        } else {
            root.pageStack.push(Qt.createComponent("org.kde.tokodon", "WelcomePage"), { application: root.application });
        }
    }

    function navigateLink(link: string, shouldOpenInternalLinks: bool): void {
        if (link.startsWith('hashtag:/') && shouldOpenInternalLinks) {
            root.pageStack.push(tagModelComponent, {
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
        crossActionDialogLoader.active = true;
        crossActionDialogLoader.item.action = action;
        crossActionDialogLoader.item.url = url;
        crossActionDialogLoader.item.open();
    }

    // Checks if the current item matches the expected pageId
    // If true, then it has sent us back to the top and you shouldn't push the same page.
    function checkIfCurrentPage(pageId: string): bool {
        if (root.pageStack.currentItem?.pageId === pageId) {
            if (root.pageStack.currentItem?.returnToTop) {
                root.pageStack.currentItem.returnToTop();
            }

            return true;
        }
        return false;
    }

    Loader {
        id: crossActionDialogLoader

        active: false

        sourceComponent: Kirigami.PromptDialog {
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

            onClosed: crossActionDialogLoader.active = false

            mainItem: ColumnLayout {
                Repeater {
                    id: accounts

                    model: AccountManager

                    delegate: Delegates.RoundedItemDelegate {
                        id: delegate

                        required property int index
                        required property string displayName
                        required property string instance
                        required property var account

                        text: displayName

                        Layout.fillWidth: true

                        contentItem: InlineIdentityInfo {
                            identity: delegate.account.identity
                        }

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
    }

    Component.onCompleted: {
        if (AccountManager.isReady) {
            startupAccountCheck();
        }
    }

    Connections {
        target: AccountManager

        function onAccountSelected(): void {
            if (AccountManager.isReady) {
                decideDefaultPage();
            }
        }

        function onAccountRemoved(): void {
            if (!AccountManager.hasAccounts) {
                root.pageStack.clear();
                root.pageStack.push(Qt.createComponent("org.kde.tokodon", "WelcomePage"), { application: root.application });
                globalDrawer.drawerOpen = false;
            }
        }

        function onAccountsReloaded(): void {
            root.pageStack.replace(mainTimeline.createObject(root), {
                name: "home"
            });
        }

        function onAccountsReady(): void {
            root.startupAccountCheck();
        }
    }

    function popoutStatusComposer(originalEditor: StatusComposer): void {
        const item = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
            closeApplicationWhenFinished: true,
            purpose: originalEditor.purpose,
            initialText: originalEditor.backend.status,
            inReplyTo: originalEditor.inReplyTo,
            previewPost: originalEditor.previewPost,
            globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
        }, {
            title: i18n("Write a New Post"),
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

    function openComposer(initialText: string, visibility: var) {
        if (Config.popOutByDefault) {
            const item = root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                closeApplicationWhenFinished: true,
                purpose: StatusComposer.New,
                initialText,
                visibility: visibility ?? AccountManager.selectedAccount.preferences.defaultVisibility
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
            root.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                purpose: StatusComposer.New,
                initialText,
                visibility: visibility ?? AccountManager.selectedAccount.preferences.defaultVisibility
            });
        }
    }

    Connections {
        target: Navigation

        function onOpenFullScreenImage(attachments: var, identity: Identity, currentIndex: int): void {
            const dialog = fullScreenImage.createObject(QQC2.Overlay.overlay, {
                attachments: attachments,
                identity: identity,
                initialIndex: currentIndex,
            });
            dialog.open();
        }

        function onOpenComposer(text: string): void {
            root.openComposer(text, undefined);
        }

        function onOpenConversation(accountId: string): void {
            root.openComposer("@" + accountId + " ", Post.Direct);
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
                const item = root.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                    purpose: StatusComposer.Reply,
                    previewPost: post
                });
                item.backend.setupReplyTo(post);
                item.refreshData();
            }
        }

        function onOpenPost(postId: string): void {
            if (!root.pageStack.currentItem.postId || root.pageStack.currentItem.postId !== postId) {
                root.pageStack.push(Qt.createComponent("org.kde.tokodon", "ThreadPage"), {
                    postId: postId,
                });
            }
        }

        function onOpenAccount(accountId: string): void {
            if (!root.pageStack.currentItem.accountId || root.pageStack.currentItem.accountId !== accountId) {
                root.pageStack.push(Qt.createComponent("org.kde.tokodon", "AccountInfo"), {
                    accountId: accountId,
                });
            }
        }

        function onOpenTag(tag: string): void {
            root.pageStack.push(tagModelComponent, {
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
            root.pageStack.push(listTimelinePage.createObject(root, {
                name,
                listId
            }));
        }
    }

    globalDrawer: Sidebar {
        id: drawer

        enabled: AccountManager.hasAccounts && AccountManager.isReady
        application: root.application
        shouldCollapse: !root.wideMode
        actions: !root.wideMode ?
            [searchAction, followRequestAction, followingAction, localTimelineAction, globalTimelineAction, conversationAction, bookmarksAction, favouritesAction, listsAction] :
            [homeAction, notificationAction, followRequestAction, followingAction, exploreAction, localTimelineAction, globalTimelineAction, conversationAction, bookmarksAction, favouritesAction, listsAction]
        bottomActions: [serverInformationAction, debugAction, moderationToolsAction, configureAction]
    }

    readonly property Kirigami.Action debugAction: Kirigami.Action {
        icon.name: "debug-run"
        onTriggered: root.pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "DebugPage"))
        visible: AccountManager.testMode
        text: i18nc("@action:button Open debug page", "Debug")
    }
    readonly property Kirigami.Action accountSwitcherAction: Kirigami.Action {
        fromQAction: root.application.action('account_switcher')
    }

    property ModerationToolsView moderationToolsView: ModerationToolsView {
        id: moderationToolsView
        window: root
    }

    property Kirigami.Action moderationToolsAction: Kirigami.Action {
        icon.name: "lock"
        text: i18nc("@action:button Open moderation tools", "Moderation Tools")
        visible: AccountManager.selectedAccount && (AccountManager.selectedAccount.identity.permission & AdminAccountInfo.ManageUsers)

        onTriggered: moderationToolsView.open()
    }

    property Component serverInformationPage: Qt.createComponent("org.kde.tokodon", "ServerInformationPage", Qt.Asynchronous)
    property Component listsPage: Qt.createComponent("org.kde.tokodon", "ListsPage", Qt.Asynchronous)
    property Component searchPage: Qt.createComponent("org.kde.tokodon", "SearchPage", Qt.Asynchronous)
    property Component conversationPage: Qt.createComponent("org.kde.tokodon", "ConversationPage", Qt.Asynchronous)
    property Component listTimelinePage: Qt.createComponent("org.kde.tokodon", "ListTimelinePage", Qt.Asynchronous)

    property Kirigami.NavigationTabBar tabBar: Kirigami.NavigationTabBar {
        id: tabbar

        // Make sure we take in count drawer width
        visible: root.pageStack.layers.depth <= 1 && AccountManager.hasAccounts && !root.wideMode && AccountManager.isReady
        enabled: !AccountManager.selectedAccountHasIssue

        contentItem: RowLayout {
            spacing: 0

            Repeater {
                model: [root.homeAction, root.notificationAction, root.exploreAction, root.profileAction]

                delegate: Kirigami.NavigationTabButton {
                    id: delegateButton

                    required property Kirigami.Action modelData

                    action: modelData

                    Layout.minimumWidth: tabbar.buttonWidth
                    Layout.maximumWidth: tabbar.buttonWidth
                    Layout.fillHeight: true

                    // Notification indicator
                    Rectangle {
                        anchors {
                            top: parent.top
                            topMargin: Kirigami.Units.mediumSpacing
                            right: parent.right
                            rightMargin: Kirigami.Units.mediumSpacing
                        }

                        color: Kirigami.Theme.highlightColor

                        width: 20
                        height: width
                        radius: width
                        visible: delegateButton.modelData.alertCount > 0

                        QQC2.Label {
                            anchors {
                                centerIn: parent
                            }

                            text: delegateButton.modelData.alertCount ?? ""
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }

    footer: !root.wideMode ? tabBar : null

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
        property string text

        parent: overlay
        opacity: text.length > 0 ? 1 : 0
        visible: !Kirigami.Settings.isMobile && !text.startsWith("hashtag:") && !text.startsWith("account:")

        z: root.globalDrawer.z + 1
        x: 0
        y: parent.height - implicitHeight

        Kirigami.Theme.colorSet: Kirigami.Theme.View

        onTextChanged: {
            // This is done so the text doesn't disappear for a split second while in the opacity transition
            if (text.length > 0) {
                linkText.text = text;
            }
        }

        Behavior on opacity {
            OpacityAnimator {
                duration: Kirigami.Units.shortDuration
                easing.type: Easing.InOutQuad
            }
        }

        contentItem: QQC2.Label {
            id: linkText
        }

        background: Kirigami.ShadowedRectangle {
            corners.topRightRadius: Kirigami.Units.cornerRadius
            color: Kirigami.Theme.backgroundColor
            border {
                color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, Kirigami.Theme.frameContrast)
                width: 1
            }
        }
    }

    Component {
        id: tagModelComponent
        TimelinePage {
            id: tagPage
            property string hashtag
            actions: Kirigami.Action {
                icon.name: tagsModel.following ?  "list-remove-user" : "list-add-user"
                text: tagsModel.following ? i18nc("@action:intoolbar", "Unfollow") : i18nc("@action:intoolbar", "Follow")
                onTriggered: tagsModel.following ? tagsModel.unfollow() : tagsModel.follow()
            }
            model: TagsTimelineModel {
                id: tagsModel

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

        Image {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            source: "qrc:/content/elephant.svg"
            LayoutMirroring.enabled: false
            asynchronous: true
        }
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
