// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.delegates 1.0 as Delegates
import QtQml.Models 2.15
import "./StatusDelegate"
import "./StatusComposer"

Kirigami.ScrollablePage {
    id: timelinePage
    title: i18n("Explore")

    property var dialog: null

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Post")
        enabled: AccountManager.hasAccounts
        onTriggered: Navigation.openStatusComposer()
    }

    TagsModel {
        id: tagsModel
    }

    MainTimelineModel {
        id: trendingPostsModel
        name: "trending"
    }


    property Kirigami.Action tendingPostsAction: Kirigami.Action {
        id: showPostsAction
        text: i18n("Trending Posts")
        checkable: true
        checked: true
        onCheckedChanged: if (checked) {
            if (tagsModel.name.length === 0) {
                trendingPostsModel.name = "trending";
            } else {
                trendingPostsModel.shouldLoadMore = false;
            }
        }
    }

    property Kirigami.Action trendingTagsAction: Kirigami.Action {
        id: showTagsAction
        text: i18n("Trending Tags")
        checkable: true
        onCheckedChanged: if (checked) {
            if (tagsModel.name.length === 0) {
                tagsModel.name = "trending";
            } else {
                tagsModel.shouldLoadMore = false;
            }
        }
    }

    header: Kirigami.NavigationTabBar {
        width: parent.width
        actions: [tendingPostsAction, trendingTagsAction]
    }

    ListView {
        id: tagsView
        model: showPostsAction.checked ? trendingPostsModel : tagsModel

        Connections {
            target: Navigation
            function onOpenFullScreenImage(attachments, identity, currentIndex) {
                if (timelinePage.isCurrentPage) {
                    timelinePage.dialog = fullScreenImage.createObject(parent, {
                        attachments: attachments,
                        identity: identity,
                        initialIndex: currentIndex,
                    });
                    timelinePage.dialog.open();
                }
            }
        }

        Connections {
            target: trendingPostsModel
            function onPostSourceReady(backend) {
                pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                    purpose: StatusComposer.Edit,
                    backend: backend
                });
            }
        }

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }

        Component {
            id: trendingPostsModelComponent

            StatusDelegate {
                timelineModel: tagsView.model
                loading: tagsView.model.loading
                showSeparator: index !== tagsView.count - 1
            }
        }

        Component {
            id: trendingTagsModelComponent

            Delegates.RoundedItemDelegate {
                id: delegate

                required property string name
                required property url url
                required property var history

                width: ListView.view.width

                onClicked: pageStack.push(tagModelComponent, { hashtag: delegate.name })

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

        delegate: showPostsAction.checked ? trendingPostsModelComponent : trendingTagsModelComponent

        QQC2.ProgressBar {
            visible: tagsView.model.loading && tagsView.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Tags")
            visible: tagsView.count === 0 && !tagsView.model.loading
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
        }
    }
}
