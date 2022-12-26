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

    property var timelineModel
    property var poll: model.poll
    property bool secondary: false
    property bool showSeparator: true
    property bool showInteractionButton: true

    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2
    highlighted: false
    hoverEnabled: false
    width: ListView.view.width
    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Separator {
            visible: showSeparator
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
        if (tootContent.hoveredLink) {
            return;
        }
        const subModel = model.threadModel;
        if (!showInteractionButton || subModel.name !== timelinePage.model.name) {
            pageStack.push("qrc:/content/ui/TimelinePage.qml", {
                model: subModel,
                type: TimelinePage.TimelineType.Thread,
            });
        }
    }
    ListView.onReused: tootContent.visible = Qt.binding(() => { return model.spoilerText.length === 0; })

    contentItem: ColumnLayout {
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            visible: model.type === Notification.Favorite
            Kirigami.Icon {
                source: "favorite"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }
            QQC2.Label {
                text: model.type === Notification.Favorite ? i18n("%1 favorited your post", model.notificationActorIdentity.displayNameHtml) : ''
                textFormat: Text.RichText
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            visible: model.pinned
            Kirigami.Icon {
                source: "pin"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }
            QQC2.Label {
                text: i18n("Pinned entry")
                color: Kirigami.Theme.disabledTextColor
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }

        RowLayout {
            visible: model.wasReblogged || model.type === Notification.Repeat
            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            Kirigami.Icon {
                source: "retweet"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: model.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.Label {
                text: model.rebloggedDisplayName ? i18n("%1 boosted", model.rebloggedDisplayName) : (model.type === Notification.Repeat ? i18n("%1 boosted your post", model.notificationActorIdentity.displayNameHtml) : '')
                color: model.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing
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
            ColumnLayout {
                Layout.fillWidth: true
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                spacing: 0
                Kirigami.Heading {
                    id: heading
                    level: 5
                    text: model.authorDisplayName
                    type: Kirigami.Heading.Type.Primary
                    color: secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                }
                Kirigami.Heading {
                    level: 5
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    color: Kirigami.Theme.disabledTextColor
                    text: `@${model.authorId}`
                    verticalAlignment: Text.AlignTop
                }
            }

            Kirigami.Heading {
                level: 5
                text: model.relativeTime
                color: secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                verticalAlignment: Text.AlignTop
                Layout.alignment: Qt.AlignTop
                elide: Text.ElideRight
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
                onLinkActivated: {
                    if (link.startsWith('hashtag:/')) {
                        const item = pageStack.push(tagModelComponent, {
                            hashtag: link.substring(9),
                        })
                    } else {
                        Qt.openUrlExternally(link)
                    }
                }
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

        QQC2.Control {
            id: attachmentGrid
            property var isSensitive: model.sensitive
            Layout.fillWidth: true
            topPadding: 0
            leftPadding: 0
            bottomPadding: 0
            rightPadding: 0
            visible: tootContent.visible && !root.secondary && model.attachments.length > 0
            contentItem: GridLayout {
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
                                if (attachmentGrid.isSensitive) {
                                    attachmentGrid.isSensitive = false;
                                } else {
                                    fullScreenImage.createObject(parent, {
                                        model: attachments,
                                        currentIndex: index,
                                    }).open();
                                }
                            }
                        }

                        Image {
                            anchors.fill: parent
                            source: visible ? "image://blurhash/" + modelData.blurhash : ''
                            visible: parent.status !== Image.Ready || attachmentGrid.isSensitive
                        }
                    }
                }
            }

            QQC2.Button {
                icon.name: "view-hidden"

                visible: !parent.isSensitive

                anchors.top: parent.top
                anchors.topMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                anchors.left: parent.left
                anchors.leftMargin: Kirigami.Units.smallSpacing

                onClicked: attachmentGrid.isSensitive = true
            }

            QQC2.Button {
                anchors.centerIn: parent

                visible: parent.isSensitive

                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                Kirigami.Theme.inherit: false

                text: i18n("Media Hidden")
                onClicked: if (attachmentGrid.isSensitive) {
                    attachmentGrid.isSensitive = false;
                } else {
                  fullScreenImage.createObject(parent, {
                        model: attachments,
                        currentIndex: 0,
                    }).open();
                }
            }
        }

        QQC2.AbstractButton {
            visible: model.card && tootContent.visible && Config.showLinkPreview && !root.secondary && model.attachments.length === 0
            Layout.fillWidth: true
            Layout.topMargin: visible ? Kirigami.Units.largeSpacing : 0
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            onClicked: Qt.openUrlExternally(model.card.url)
            HoverHandler {
                cursorShape: Qt.PointingHandCursor
                onHoveredChanged: if (hovered) {
                    applicationWindow().hoverLinkIndicator.text = model.card.url;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }
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

        ColumnLayout {
            QQC2.ButtonGroup {
                id: pollGroup
                exclusive: poll !== undefined && !poll.multiple
            }

            Repeater {
                model: poll !== undefined && poll.voted ? poll.options : []
                ColumnLayout {
                    RowLayout {
                        QQC2.Label {
                            text: modelData.votesCount !== -1 ? i18nc("Votes percentage", "%1%", modelData.votesCount / poll.votesCount * 100) : ''
                            Layout.alignment: Qt.AlignVCenter
                            Layout.minimumWidth: Kirigami.Units.gridUnit * 2
                        }

                        QQC2.Label {
                            text: modelData.title
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }

                    QQC2.ProgressBar {
                        from: 0
                        to: 100
                        value: modelData.votesCount / poll.votesCount * 100
                        Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 10
                        Layout.alignment: Qt.AlignVCenter
                    }
                }
            }

            Repeater {
                model: poll !== undefined && !poll.voted ? poll.options : []
                RowLayout {
                    QQC2.CheckBox {
                        visible: poll.multiple
                        Layout.alignment: Qt.AlignVCenter
                        QQC2.ButtonGroup.group: pollGroup
                        property int choiceIndex: index
                    }

                    QQC2.RadioButton {
                        visible: !poll.multiple
                        Layout.alignment: Qt.AlignVCenter
                        QQC2.ButtonGroup.group: pollGroup
                        property int choiceIndex: index
                    }

                    QQC2.Label {
                        text: modelData.title
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                    }
                }
            }

            QQC2.Button {
                visible: poll !== undefined && !poll.voted
                text: i18n("Vote")
                enabled: pollGroup.checkState !== Qt.Unchecked
                onClicked: {
                    let choices = [];
                    const buttons = pollGroup.buttons;
                    for (let i in buttons) {
                        const button = buttons[i];
                        if (!button.visible) {
                            continue;
                        }

                        if (button.checked) {
                            choices.push(button.choiceIndex);
                        }
                    }
                    timelineModel.actionVote(timelineModel.index(model.index, 0), choices)
                }
            }
        }

        RowLayout {
            visible: showInteractionButton
            Layout.topMargin: Kirigami.Units.largeSpacing
            InteractionButton {
                iconSource: "qrc:/content/icon/reply-post.svg"
                text: model.repliesCount < 2 ? model.repliesCount : (Config.showPostStats ? model.repliesCount : i18nc("More than one reply", "1+"))
                onClicked: {
                    const post = AccountManager.selectedAccount.newPost()
                    post.inReplyTo = model.id;
                    post.mentions = model.mentions;
                    post.visibility = model.visibility;
                    if (!post.mentions.includes(`@${model.authorId}`)) {
                        post.mentions.push(`@${model.authorId}`);
                    }
                    pageStack.layers.push("qrc:/content/ui/TootComposer.qml", {
                        postObject: post,
                    });
                }
                QQC2.ToolTip.text: i18nc("Reply to a post", "Reply")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            InteractionButton {
                iconSource: model.reblogged ? 'qrc:/content/icon/boost-post-done.svg' : 'qrc:/content/icon/boost-post.svg'
                interacted: model.reblogged
                interactionColor: "green"
                onClicked: timelineModel.actionRepeat(timelineModel.index(model.index, 0))
                text: Config.showPostStats ? model.reblogsCount : ''
                QQC2.ToolTip.text: i18nc("Share a post", "Boost")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            InteractionButton {
                iconSource: model.favorite ? 'qrc:/content/icon/like-post-done.svg' : 'qrc:/content/icon/like-post.svg'
                interacted: model.favorite
                interactionColor: "orange"
                onClicked: timelineModel.actionFavorite(timelineModel.index(model.index, 0))
                text: Config.showPostStats ? favoritesCount : ''
                QQC2.ToolTip.text: i18nc("Like a post", "Like")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }
    }
}
