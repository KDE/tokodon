// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private
import Qt.labs.qmlmodels 1.0

import "../Components"

/**
 * @brief The attachment grid used in statuses, which is on a specialized grid layout
 */
QQC2.Control {
    id: root

    required property var attachments
    required property var identity
    required property bool sensitive
    required property bool secondary
    required property bool expandedPost
    required property bool inViewPort
    required property real viewportWidth

    property bool canHideMedia: true

    // Only uncrop timeline media if requested by the user, and there's only one attachment
    // Expanded posts (like in threads) are always uncropped.
    readonly property var shouldKeepAspectRatio: (!Config.cropMedia || root.expandedPost) && root.attachments.length === 1

    property bool isSensitive: (AccountManager.selectedAccount.preferences.extendMedia === "hide_all" ? true : (AccountManager.selectedAccount.preferences.extendMedia === "show_all" ? false : root.sensitive))
    signal userSensitivityChanged(hide: bool)

    property bool hasValidAttachment: {
        for (const i in root.attachments) {
            if (root.attachments[i].attachmentType !== Attachment.Unknown) {
                return true;
            }
        }
        return false;
    }

    function showMedia(): void {
        isSensitive = false;
        userSensitivityChanged(false);
    }

    function hideMedia(): void {
        isSensitive = true;
        userSensitivityChanged(true);
    }

    function openAttachmentMenu(attachment: Attachment): void {
        imageMenu.active = true;
        imageMenu.item.attachment = attachment;
        imageMenu.item.popup();
    }

    Layout.fillWidth: true
    Layout.fillHeight: shouldKeepAspectRatio
    Layout.topMargin: Kirigami.Units.largeSpacing

    Accessible.description: {
        if (root.attachments.length === 0) {
            return "";
        }

        switch (root.attachments[0].attachmentType) {
            case Attachment.Image:
                return i18n("Status with image attachment");
            case Attachment.GifV:
                return i18n("Status with GifV attachment");
            case Attachment.Video:
                return i18n("Status with Video attachment");
        }
    }

    topPadding: 0
    leftPadding: 0
    bottomPadding: 0
    rightPadding: 0

    contentItem: GridLayout {
        id: attachmentGridLayout

        columns: Math.min(root.attachments.length, 2)
        rowSpacing: Kirigami.Units.smallSpacing
        columnSpacing: Kirigami.Units.smallSpacing

        Repeater {
            id: attachmentsRepeater
            model: root.secondary ? [] : attachments

            DelegateChooser {
                role: "attachmentType"

                DelegateChoice {
                    roleValue: Attachment.Image

                    MediaContainer {
                        id: imgContainer

                        required property var modelData

                        count: attachmentsRepeater.count
                        shouldKeepAspectRatio: root.shouldKeepAspectRatio
                        rootWidth: root.viewportWidth
                        gridLayout: attachmentGridLayout

                        sourceWidth: modelData.sourceWidth > img.sourceSize.width ? modelData.sourceWidth : img.sourceSize.width
                        sourceHeight: modelData.sourceHeight > img.sourceSize.height ? modelData.sourceHeight : img.sourceSize.height

                        onClicked: {
                            if (root.isSensitive) {
                                root.showMedia();
                            } else {
                                Navigation.openFullScreenImage(root.attachments, root.identity, imgContainer.index);
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        Accessible.description: modelData.caption

                        FocusedImage {
                            id: img

                            anchors.fill: parent
                            source: modelData.previewUrl

                            onStatusChanged: {
                                if (status === Image.Error) {
                                    // Fall back to remote URL
                                    img.source = modelData.remoteUrl;
                                }
                            }

                            crop: !root.shouldKeepAspectRatio
                            focusX: modelData.focusX
                            focusY: modelData.focusY

                            Rectangle {
                                anchors.fill: parent
                                color: "black"

                                Image {
                                    anchors.fill: parent

                                    source: visible ? modelData.tempSource : ''
                                }

                                visible: parent.status !== Image.Ready || root.isSensitive
                            }

                            QQC2.Button {
                                anchors.centerIn: parent

                                visible: modelData.attachmentType === Attachment.Unknown
                                text: i18n("Not available")

                                onClicked: Qt.openUrlExternally(modelData.remoteUrl)
                            }
                        }
                    }
                }

                DelegateChoice {
                    roleValue: Attachment.GifV

                    VideoAttachment {
                        id: video

                        required property var modelData

                        count: attachmentsRepeater.count
                        shouldKeepAspectRatio: root.shouldKeepAspectRatio
                        rootWidth: root.viewportWidth
                        gridLayout: attachmentGridLayout

                        videoUrl: modelData.source
                        previewUrl: modelData.previewUrl
                        autoPlay: Config.autoPlayGif && !root.isSensitive
                        isSensitive: root.isSensitive
                        showControls: false
                        looping: true
                        showGifChip: true

                        onClicked: {
                            if (root.isSensitive) {
                                root.showMedia();
                            } else {
                                video.togglePlayPause()
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        Accessible.description: modelData.caption

                        Connections {
                            target: root
                            function onInViewPortChanged() {
                                if (!root.inViewPort) {
                                    video.pause();
                                } else if(video.autoPlay) {
                                    video.play();
                                }
                            }
                            function onUserSensitivityChanged(hide) {
                                if (hide) {
                                    video.pause()
                                } else if (video.autoPlay) {
                                    video.play()
                                }
                            }
                        }
                    }
                }

                DelegateChoice {
                    roleValue: Attachment.Video

                    VideoAttachment {
                        id: video

                        required property var modelData

                        count: attachmentsRepeater.count
                        shouldKeepAspectRatio: root.shouldKeepAspectRatio
                        rootWidth: root.viewportWidth
                        gridLayout: attachmentGridLayout

                        videoUrl: modelData.source
                        previewUrl: modelData.previewUrl
                        autoPlay: false
                        isSensitive: root.isSensitive
                        looping: false
                        showVideoChip: true

                        Accessible.description: modelData.caption

                        onClicked: {
                            if (root.isSensitive) {
                                root.showMedia();
                            } else {
                                video.togglePlayPause()
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        Connections {
                            target: root
                            function onInViewPortChanged() {
                                if (!root.inViewPort) {
                                    video.pause();
                                }
                            }
                            function onUserSensitivityChanged(hide) {
                                video.pause()
                            }
                        }
                    }
                }
            }
        }
    }

    QQC2.Button {
        anchors {
            top: parent.top
            topMargin: Kirigami.Units.smallSpacing
            left: parent.left
            leftMargin: Kirigami.Units.smallSpacing
        }

        visible: !parent.isSensitive && parent.hasValidAttachment && root.canHideMedia
        icon.name: "view-hidden"
        text: i18nc("@action:button", "Hide Media")
        display: QQC2.Button.IconOnly
        activeFocusOnTab: false

        QQC2.ToolTip.text: text
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        onClicked: root.hideMedia()
    }

    QQC2.Button {
        anchors.centerIn: parent

        visible: parent.isSensitive && parent.hasValidAttachment
        text: i18n("Media Hidden")

        onClicked: root.showMedia()
    }

    Loader {
        id: imageMenu

        active: false
        visible: active

        sourceComponent: AttachmentMenu {
            onClosed: postMenu.active = false
        }
    }
}
