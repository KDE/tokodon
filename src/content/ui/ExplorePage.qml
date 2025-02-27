// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

import org.kde.kirigamiaddons.delegates 1 as Delegates
import QtQml.Models
import "./PostDelegate"
import "./StatusComposer"

Kirigami.Page {
    id: timelinePage
    title: i18n("Explore")

    property var dialog: null

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    actions: Kirigami.Action {
        icon.name: "list-add"
        text: i18nc("@action:button", "Post")
        enabled: AccountManager.hasAccounts
        onTriggered: Navigation.openComposer("")
    }

    property Kirigami.Action trendingPostsAction: Kirigami.Action {
        text: i18n("Posts")
        icon.name: "view-conversation-balloon-symbolic"
        checkable: true
    }

    property Kirigami.Action trendingTagsAction: Kirigami.Action {
        text: i18n("Hashtags")
        icon.name: "tag-symbolic"
        checkable: true
    }

    property Kirigami.Action trendingNewsAction: Kirigami.Action {
        text: i18n("News")
        icon.name: "view-pim-news-symbolic"
        checkable: true
    }

    property Kirigami.Action suggestedUsersAction: Kirigami.Action {
        text: i18n("Users")
        icon.name: "system-users-symbolic"
        checkable: true
    }

    header: Kirigami.NavigationTabBar {
        anchors.left: parent.left
        anchors.right: parent.right
        actions: [trendingPostsAction, trendingTagsAction, trendingNewsAction, suggestedUsersAction]

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
    }

    Component.onCompleted: {
        trendingPostsAction.checked = true

        // TODO: When we can require KF 6.8, set it as a normal property
        if (timelinePage.verticalScrollBarInteractive !== undefined) {
            timelinePage.verticalScrollBarInteractive = false;
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    StackLayout {
        anchors.fill: parent

        currentIndex: {
            if (suggestedUsersAction.checked) {
                return 3;
            } else if (trendingNewsAction.checked) {
                return 2;
            } else if (trendingTagsAction.checked) {
                return 1;
            }
            return 0;
        }

        QQC2.ScrollView {
            clip: true

            Keys.onPressed: event => timelineView.handleKeyEvent(event)

            QQC2.ScrollBar.vertical.interactive: false

            TimelineView {
                id: timelineView

                model: MainTimelineModel {
                    id: trendingPostsModel
                    name: "trending"
                    shouldLoadMore: trendingPostsAction.checked
                }

                Kirigami.PlaceholderMessage {
                    icon.name: "view-conversation-balloon-symbolic"
                    text: i18nc("@info:placeholder", "No Posts")
                    visible: !trendingPostsModel.loading && timelineView.count === 0
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }
        QQC2.ScrollView {
            clip: true

            ListView {
                id: tagsView

                model: TagsModel {
                    id: tagsModel
                    name: "trending"
                    shouldLoadMore: trendingTagsAction.checked
                }

                Kirigami.LoadingPlaceholder {
                    visible: tagsView.model.loading && tagsView.count === 0
                    anchors.centerIn: parent
                }

                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    icon.name: "tag-symbolic"
                    text: i18nc("@info:placeholder", "No Tags")
                    visible: !tagsView.model.loading && tagsView.count === 0
                    width: parent.width - (Kirigami.Units.largeSpacing * 4)
                }

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property string name
                    required property url url
                    required property var history

                    width: ListView.view.width

                    onClicked: pageStack.push(tagModelComponent, {hashtag: delegate.name})

                    contentItem: ColumnLayout {
                        Kirigami.Heading {
                            level: 4
                            text: `#${delegate.name}`
                            type: Kirigami.Heading.Type.Primary
                            verticalAlignment: Text.AlignTop
                            elide: Text.ElideRight
                            textFormat: Text.StyledText
                            Layout.fillWidth: true
                        }

                        QQC2.Label {
                            font.pixelSize: Config.defaultFont.pixelSize + 1
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: Kirigami.Theme.disabledTextColor
                            text: i18np("%1 person is talking", "%1 people are talking", delegate.history[0].accounts)
                            verticalAlignment: Text.AlignTop
                        }
                    }
                }
            }
        }
        QQC2.ScrollView {
            clip: true

            ListView {
                id: newsView

                model: TrendingNewsModel {
                    id: newsModel
                    shouldLoadMore: trendingNewsAction.checked
                }

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property string title
                    required property url url
                    required property string description
                    required property string image

                    width: ListView.view.width

                    onClicked: {
                        pageStack.push(linkTimeline.createObject(root, {
                            url: delegate.url
                        }));
                    }

                    Component {
                        id: linkTimeline
                        TimelinePage {
                            id: timelinePage

                            property alias url: timelineModel.url

                            model: MainTimelineModel {
                                id: timelineModel
                                name: "link"
                                showReplies: timelinePage.showReplies
                                showBoosts: timelinePage.showBoosts
                            }
                        }
                    }

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Image {
                            source: delegate.image

                            layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
                            layer.effect: RoundedEffect {}

                            Layout.preferredHeight: 50
                            Layout.preferredWidth: 50
                        }

                        ColumnLayout {
                            spacing: Kirigami.Units.smallSpacing

                            Kirigami.Heading {
                                level: 4
                                text: delegate.title
                                type: Kirigami.Heading.Type.Primary
                                verticalAlignment: Text.AlignTop
                                elide: Text.ElideRight
                                textFormat: Text.StyledText
                                maximumLineCount: 1
                                Layout.fillWidth: true
                            }

                            QQC2.Label {
                                font.pixelSize: Config.defaultFont.pixelSize + 1
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                color: Kirigami.Theme.disabledTextColor
                                text: delegate.description
                                maximumLineCount: 2
                                verticalAlignment: Text.AlignTop
                            }
                        }
                    }
                }

                Kirigami.PlaceholderMessage {
                    icon.name: "view-pim-news-symbolic"
                    text: i18nc("@info:placeholder", "No News")
                    visible: !newsModel.loading && newsView.count === 0
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }
        QQC2.ScrollView {
            clip: true

            ListView {
                id: suggestionsView

                model: SuggestionsModel {
                    id: usersModel
                    shouldLoadMore: suggestedUsersAction.checked
                }

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property var identity

                    width: ListView.view.width

                    onClicked: Qt.openUrlExternally(url)

                    contentItem: UserCard {
                        userIdentity: delegate.identity

                        Kirigami.Theme.colorSet: Kirigami.Theme.Window
                        Kirigami.Theme.inherit: false
                    }
                }

                Kirigami.PlaceholderMessage {
                    icon.name: "system-users-symbolic"
                    text: i18nc("@info:placeholder", "No Users")
                    visible: !usersModel.loading && suggestionsView.count === 0
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }
    }
}
