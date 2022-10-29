// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.BasicListItem {
    topPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    highlighted: false
    hoverEnabled: false
    property bool secondary: false
    onClicked: {
        const subModel = model.threadModel;
        if (subModel.name !== timelinePage.model.name) {
            pageStack.push("qrc:/content/ui/TimelinePage.qml", {
                    "model": subModel
                });
        }
    }
    ListView.onReused: tootContent.visible = Qt.binding(() => {
            return model.spoilerText.length === 0;
        })
    contentItem: GridLayout {
        columnSpacing: Kirigami.Units.largeSpacing
        rowSpacing: 0
        columns: 2

        Kirigami.Icon {
            source: "favorite"
            Layout.alignment: Qt.AlignRight
            visible: model.type === Notification.Favorite
            color: Kirigami.Theme.disabledTextColor
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }
        QQC2.Label {
            text: model.type === Notification.Favorite ? i18n("%1 favorited your post", model.actorDisplayName) : ''
            visible: model.type === Notification.Favorite
            font: Kirigami.Theme.smallFont
        }

        Kirigami.Icon {
            source: "pin"
            Layout.alignment: Qt.AlignRight
            visible: model.pinned && timelinePage.isProfile
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }
        QQC2.Label {
            text: i18n("Pinned entry")
            visible: model.pinned && timelinePage.isProfile
            color: Kirigami.Theme.disabledTextColor
            font: Kirigami.Theme.smallFont
        }

        Kirigami.Icon {
            source: "retweet"
            Layout.alignment: Qt.AlignRight
            visible: model.wasReblogged || model.type === Notification.Repeat
            color: model.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }
        QQC2.Label {
            visible: model.wasReblogged || model.type === Notification.Repeat
            text: model.rebloggedDisplayName ? i18n("%1 boosted", model.rebloggedDisplayName) : (model.type === Notification.Repeat ? i18n("%1 boosted your post", model.actorDisplayName) : '')
            color: model.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            font: Kirigami.Theme.smallFont
        }

        Kirigami.Avatar {
            Layout.alignment: Qt.AlignTop
            Layout.rowSpan: 5
            source: model.avatar
            cache: true
            TapHandler {
                onTapped: {
                    pageStack.push("qrc:/content/ui/AccountInfo.qml", {
                            "model": model.accountModel
                        });
                }
            }
            name: model.authorDisplayName
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing
            QQC2.Label {
                text: model.authorDisplayName
                color: secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
            }
            QQC2.Label {
                Layout.fillWidth: true
                elide: Text.ElideRight
                color: Kirigami.Theme.disabledTextColor
                text: `@${model.authorId}`
            }
            QQC2.Label {
                text: model.relativeTime
                color: secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
            }
        }
        ColumnLayout {
            RowLayout {
                visible: model.spoilerText.length !== 0
                QQC2.Label {
                    Layout.fillWidth: true
                    text: model.spoilerText
                    wrapMode: Text.Wrap
                }
                QQC2.Button {
                    text: i18n("Show more")
                    onClicked: tootContent.visible = !tootContent.visible
                }
            }
            TextEdit {
                id: tootContent
                Layout.fillWidth: true
                text: model.display
                textFormat: TextEdit.RichText
                wrapMode: Text.Wrap
                visible: model.spoilerText.length === 0
                readOnly: true
                selectByMouse: !Kirigami.Settings.isMobile
                // TODO handle opening profile page in tokodon
                onLinkActivated: Qt.openUrlExternally(link)
                color: secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
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

        GridLayout {
            id: attachmentGrid
            visible: tootContent.visible
            Layout.fillWidth: true
            columns: model.attachments.length > 1 ? 2 : 1
            Repeater {
                model: attachments
                Image {
                    Layout.fillWidth: true
                    Layout.maximumWidth: sourceSize.width
                    Layout.maximumHeight: width / sourceSize.width * sourceSize.height
                    source: modelData.previewUrl
                    cache: true
                    TapHandler {
                        onTapped: {
                            fullScreenImage.createObject(parent, {
                                    "model": attachments,
                                    "currentIndex": index
                                }).showFullScreen();
                        }
                    }
                }
            }
        }

        Kirigami.AbstractCard {
            visible: model.card && tootContent.visible && Config.showLinkPreview
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing
            TapHandler {
                onTapped: Qt.openUrlExternally(model.card.url)
            }
            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }
            contentItem: RowLayout {
                QQC2.Pane {
                    visible: model.card && model.card.image
                    Kirigami.Theme.colorSet: Kirigami.Theme.Window
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 3
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 3
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    Image {
                        anchors.centerIn: parent
                        sourceSize.width: Kirigami.Units.gridUnit * 3
                        sourceSize.height: Kirigami.Units.gridUnit * 3
                        source: model.card ? model.card.image : ''
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    Kirigami.Heading {
                        level: 3
                        text: model.card ? model.card.title : ''
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        HoverHandler {
                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                    QQC2.Label {
                        text: model.card ? model.card.providerName : ''
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        HoverHandler {
                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                }
            }
        }

        RowLayout {
            QQC2.ToolButton {
                icon.name: "mail-replied-symbolic"
                icon.width: Kirigami.Units.iconSizes.smallMedium
                icon.height: Kirigami.Units.iconSizes.smallMedium
                text: model.repliesCount < 2 || Config.showPostStats ? model.repliesCount : "1+"
                onClicked: {
                    const post = AccountManager.selectedAccount.newPost();
                    post.inReplyTo = model.id;
                    post.mentions = model.mentions;
                    if (!post.mentions.includes(`@${model.authorId}`)) {
                        post.mentions.push(`@${model.authorId}`);
                    }
                    pageStack.layers.push("qrc:/content/ui/TootComposer.qml", {
                            "postObject": post
                        });
                }
                QQC2.ToolTip {
                    text: i18nc("Reply to a post", "Reply")
                }
            }
            QQC2.ToolButton {
                icon.source: model.reblogged ? 'qrc:/content/icon/boost-post-done.svg' : 'qrc:/content/icon/boost-post.svg'
                icon.width: Kirigami.Units.iconSizes.smallMedium
                icon.height: Kirigami.Units.iconSizes.smallMedium
                onClicked: timelineModel.actionRepeat(timelineModel.index(model.index, 0))
                text: Config.showPostStats ? model.reblogsCount : ''
                QQC2.ToolTip {
                    text: i18nc("Share a post", "Boost")
                }
            }
            QQC2.ToolButton {
                icon.source: model.favorite ? 'qrc:/content/icon/like-post-done.svg' : 'qrc:/content/icon/like-post.svg'
                icon.width: Kirigami.Units.iconSizes.smallMedium
                icon.height: Kirigami.Units.iconSizes.smallMedium
                onClicked: timelineModel.actionFavorite(timelineModel.index(model.index, 0))
                text: Config.showPostStats ? favoritesCount : ''
                QQC2.ToolTip {
                    text: i18nc("Like a post", "Like")
                }
            }
        }
    }
}
