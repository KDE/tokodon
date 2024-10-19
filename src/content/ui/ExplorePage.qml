// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import org.kde.tokodon.private
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

    property Kirigami.Action tendingPostsAction: Kirigami.Action {
        text: i18n("Posts")
        icon.name: "tokodon-chat-reply"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked) {
                if (tagsModel.name.length === 0) {
                    trendingPostsModel.name = "trending";
                } else {
                    trendingPostsModel.shouldLoadMore = false;
                }
            }
        }
    }

    property Kirigami.Action trendingTagsAction: Kirigami.Action {
        text: i18n("Tags")
        icon.name: "tag-symbolic"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked) {
                if (tagsModel.name.length === 0) {
                    tagsModel.name = "trending";
                } else {
                    tagsModel.shouldLoadMore = false;
                }
            }
        }
    }

    header: Kirigami.NavigationTabBar {
        anchors.left: parent.left
        anchors.right: parent.right
        actions: [tendingPostsAction, trendingTagsAction]

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
    }

    Component.onCompleted: {
        tendingPostsAction.checked = true

        // TODO: When we can require KF 6.8, set it as a normal property
        if (timelinePage.verticalScrollBarInteractive !== undefined) {
            timelinePage.verticalScrollBarInteractive = false;
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    StackLayout {
        anchors.fill: parent

        currentIndex: tendingPostsAction.checked ? 0 : 1

        QQC2.ScrollView {
            clip: true

            Keys.onPressed: event => timelineView.handleKeyEvent(event)

            QQC2.ScrollBar.vertical.interactive: false

            TimelineView {
                id: timelineView

                model: MainTimelineModel {
                    id: trendingPostsModel
                    name: "trending"
                }
            }
        }
        QQC2.ScrollView {
            clip: true

            ListView {
                id: tagsView

                model: TagsModel {
                    id: tagsModel
                }

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property string name
                    required property url url
                    required property var history

                    width: ListView.view.width

                    onClicked: pageStack.push(tagModelComponent, {hashtag: delegate.name})

                    QQC2.ProgressBar {
                        visible: tagsView.model.loading && tagsView.count === 0
                        anchors.centerIn: parent
                        indeterminate: true
                    }

                    Kirigami.PlaceholderMessage {
                        anchors.centerIn: parent
                        text: i18nc("@label", "No Trending Tags")
                        visible: !tagsView.model.loading && tagsView.count === 0
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)
                    }

                    contentItem: ColumnLayout {
                        Kirigami.Heading {
                            level: 4
                            text: `#${delegate.name}`
                            type: Kirigami.Heading.Type.Primary
                            verticalAlignment: Text.AlignTop
                            elide: Text.ElideRight
                            textFormat: Text.RichText
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
    }
}
