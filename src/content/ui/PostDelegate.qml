// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import QtGraphicalEffects 1.0

QQC2.ItemDelegate {
    id: root
    topPadding: Kirigami.Units.largeSpacing * 2
    bottomPadding: Kirigami.Units.largeSpacing * 2
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2
    highlighted: false
    hoverEnabled: false
    property bool secondary: false
    width: ListView.view.width
    background: Item {
        Kirigami.Separator {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
            }
        }
    }
    bottomInset: 2
    onClicked: {
        const subModel = model.threadModel;
        if (subModel.name !== timelinePage.model.name) {
            pageStack.push("qrc:/content/ui/TimelinePage.qml", {
                model: subModel,
                type: TimelinePage.TimelineType.Thread,
            });
        }
    }
    ListView.onReused: tootContent.visible = Qt.binding(() => { return model.spoilerText.length === 0; })
    contentItem: GridLayout {
        columnSpacing: Kirigami.Units.largeSpacing * 2
        rowSpacing: 0
        columns: 2

        Kirigami.Icon {
            source: "favorite"
            Layout.alignment: Qt.AlignRight
            visible: model.type === Notification.Favorite
            color: Kirigami.Theme.disabledTextColor
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.largeSpacing
        }
        QQC2.Label {
            text: model.type === Notification.Favorite ? i18n("%1 favorited your post", model.actorDisplayName) : ''
            visible: model.type === Notification.Favorite
            Layout.bottomMargin: Kirigami.Units.largeSpacing
        }

        Kirigami.Icon {
            source: "pin"
            Layout.alignment: Qt.AlignRight
            visible: model.pinned && timelinePage.isProfile
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.largeSpacing
        }
        QQC2.Label {
            text: i18n("Pinned entry")
            visible: model.pinned && timelinePage.isProfile
            color: Kirigami.Theme.disabledTextColor
            Layout.bottomMargin: Kirigami.Units.largeSpacing
        }

        Kirigami.Icon {
            source: "retweet"
            Layout.alignment: Qt.AlignRight
            visible: model.wasReblogged || model.type === Notification.Repeat
            color: model.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            Layout.bottomMargin: Kirigami.Units.largeSpacing
        }

        QQC2.Label {
            visible: model.wasReblogged || model.type === Notification.Repeat
            text: model.rebloggedDisplayName ? i18n("%1 boosted", model.rebloggedDisplayName) : (model.type === Notification.Repeat ? i18n("%1 boosted your post", model.actorDisplayName) : '')
            color: model.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            Layout.bottomMargin: Kirigami.Units.largeSpacing
        }

        Kirigami.Avatar {
            Layout.alignment: Qt.AlignTop
            Layout.rowSpan: 5
            source: model.avatar
            cache: true
            actions.main: Kirigami.Action {
                onTriggered: pageStack.push("qrc:/content/ui/AccountInfo.qml", {
                    model: model.accountModel,
                })
            }
            name: model.authorDisplayName
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing
            Kirigami.Heading {
                id: heading
                level: 5
                text: model.authorDisplayName
                type: Kirigami.Heading.Type.Primary
                color: secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
            }
            Kirigami.Heading {
                level: 5
                Layout.fillWidth: true
                elide: Text.ElideRight
                color: Kirigami.Theme.disabledTextColor
                text: `@${model.authorId}`
            }
            Kirigami.Heading {
                level: 5
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
                font.pointSize: heading.font.pointSize
                Layout.fillWidth: true
                text: "<style>
a{
    color: " + Kirigami.Theme.linkColor + ";
    text-decoration: none;
}
</style>" + model.display
                textFormat: TextEdit.RichText
                wrapMode: Text.Wrap
                visible: model.spoilerText.length === 0
                readOnly: true
                selectByMouse: !Kirigami.Settings.isMobile
                // TODO handle opening profile page in tokodon
                onLinkActivated: Qt.openUrlExternally(link)
                color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                onHoveredLinkChanged: if (hoveredLink.length > 0) {
                    applicationWindow().hoverLinkIndicator.text = hoveredLink;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }

                TapHandler {
                    onTapped: root.clicked()
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
            visible: tootContent.visible && !root.secondary
            Layout.fillWidth: true
            columns: model.attachments.length > 1 ? 2 : 1
            Repeater {
                id: attachmentsRepeater
                model: root.secondary ? [] : attachments
                Image {
                    id: img
                    Layout.fillWidth: true
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    //Layout.maximumWidth: sourceSize.width
                    Layout.maximumHeight: Math.min(width / sourceSize.width * sourceSize.height, attachmentsRepeater.count === 1 ? Kirigami.Units.gridUnit * 18 : Kirigami.Units.gridUnit * 10)
                    source: modelData.previewUrl
                    mipmap: true
                    cache: true
                    fillMode: Image.PreserveAspectCrop
                    layer.enabled: true
                    layer.effect: OpacityMask {
                        maskSource: Item {
                            width: img.width
                            height: img.height
                            Rectangle {
                                anchors.centerIn: parent
                                width: img.width
                                height: img.height
                                radius: Kirigami.Units.smallSpacing
                            }
                        }
                    }
                    TapHandler {
                        onTapped: {
                            fullScreenImage.createObject(parent, {
                                model: attachments,
                                currentIndex: index
                            }).open()
                        }
                    }
                }
            }
        }

        QQC2.AbstractButton {
            visible: model.card && tootContent.visible && Config.showLinkPreview && !root.secondary && model.attachments.length === 0
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            onClicked: Qt.openUrlExternally(model.card.url)
            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }
            background: Rectangle {
                radius: Kirigami.Units.largeSpacing
                color: 'transparent'
                border {
                    width: 1
                    color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor,0.15)
                }
            }
            contentItem: RowLayout {
                Rectangle {
                    id: logo
                    visible: model.card && model.card.image
                    color: Kirigami.Theme.backgroundColor
                    Kirigami.Theme.colorSet: Kirigami.Theme.Window
                    radius: Kirigami.Units.largeSpacing
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 3
                    Layout.maximumHeight: Kirigami.Units.gridUnit * 3
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 3
                    Layout.topMargin: 1
                    Layout.bottomMargin: 1
                    Layout.leftMargin: 1
                    Image {
                        id: img
                        mipmap: true
                        smooth: true
                        sourceSize {
                            width: logo.width
                            height: logo.height
                        }

                        layer.enabled: true
                        layer.effect: OpacityMask {
                            maskSource: Item {
                                width: img.width
                                height: img.height
                                Kirigami.ShadowedRectangle {
                                    anchors.centerIn: parent
                                    corners {
                                        bottomLeftRadius: Kirigami.Units.largeSpacing + 1
                                        topLeftRadius: Kirigami.Units.largeSpacing + 1
                                    }
                                    width: img.width
                                    height: img.height
                                }
                            }
                        }

                        fillMode: Image.PreserveAspectCrop
                        anchors {
                            fill: parent
                        }
                        source: model.card ? model.card.image : ''
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.smallSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Kirigami.Heading {
                        level: 4
                        text: model.card ? model.card.title : ''
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        wrapMode: model.card && model.card.providerName ? Text.WordWrap : Text.NoWrap
                        maximumLineCount: 2
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
            Layout.topMargin: Kirigami.Units.largeSpacing
            InteractionButton {
                iconSource: "qrc:/content/icon/reply-post.svg"
                text: model.repliesCount < 2 || Config.showPostStats ? model.repliesCount : "1+"
                onClicked: {
                    const post = AccountManager.selectedAccount.newPost()
                    post.inReplyTo = model.id;
                    post.mentions = model.mentions;
                    if (!post.mentions.includes(`@${model.authorId}`)) {
                        post.mentions.push(`@${model.authorId}`);
                    }
                    pageStack.layers.push("qrc:/content/ui/TootComposer.qml", {
                        postObject: post
                    });
                }
                QQC2.ToolTip {
                    text: i18nc("Reply to a post", "Reply")
                }
            }
            InteractionButton {
                iconSource: model.reblogged ? 'qrc:/content/icon/boost-post-done.svg' : 'qrc:/content/icon/boost-post.svg'
                interacted: model.reblogged
                interactionColor: "green"
                onClicked: timelineModel.actionRepeat(timelineModel.index(model.index, 0))
                text: Config.showPostStats ? model.reblogsCount : ''
                QQC2.ToolTip {
                    text: i18nc("Share a post", "Boost")
                }
            }
            InteractionButton {
                iconSource: model.favorite ? 'qrc:/content/icon/like-post-done.svg' : 'qrc:/content/icon/like-post.svg'
                interacted: model.favorite
                interactionColor: "orange"
                onClicked: timelineModel.actionFavorite(timelineModel.index(model.index, 0))
                text: Config.showPostStats ? favoritesCount : ''
                QQC2.ToolTip {
                    text: i18nc("Like a post", "Like")
                }
            }
        }
    }
}
