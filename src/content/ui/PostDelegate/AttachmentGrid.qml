// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

import Qt.labs.qmlmodels 1.0

import "../Components"

/**
 * @brief The attachment grid used in statuses, which is on a specialized grid layout
 */
Item {
    id: root

    required property var attachments
    required property var identity
    required property bool sensitive
    required property bool secondary
    required property bool expandedPost
    required property bool inViewPort
    required property real viewportWidth

    property bool canHideMedia: true
    property bool forceCrop: false

    // Only uncrop timeline media if requested by the user, and there's only one attachment
    // Expanded posts (like in threads) are always uncropped.
    readonly property var shouldKeepAspectRatio: (!Config.cropMedia || root.expandedPost) && root.attachments.length === 1 && !forceCrop

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

    property double firstAttachmentAspectRatio: {
        if (root.attachments.length === 0) {
            return 0.0;
        }

        const firstAttachment = root.attachments[0];
        if (firstAttachment.sourceHeight === 0) {
            return 1.0;
        }

        const aspectRatio = firstAttachment.sourceHeight / Math.max(firstAttachment.sourceWidth, 1)
        return aspectRatio;
    }

    function showMedia(): void {
        isSensitive = false;
        userSensitivityChanged(false);
        // Switch the focus to the first attachment so the screen reader repeats it's actual alt text.
        root.forceActiveFocus(Qt.OtherFocusReason);
        attachmentsRepeater.itemAt(0).forceActiveFocus(Qt.OtherFocusReason);
    }

    function hideMedia(): void {
        isSensitive = true;
        userSensitivityChanged(true);
    }

    function openAttachmentMenu(attachment: Attachment): void {
        imageMenu.active = true;
        imageMenu.item.attachment = attachment;
        imageMenu.item.popup(QQC2.ApplicationWindow.window);
    }

    function isSpecialAttachment(count: int, index: int): bool {
        return index === 0 && count === 3;
    }

    readonly property real aspectRatio: 9.0 / 16.0
    implicitHeight: shouldKeepAspectRatio ? Math.ceil(viewportWidth * firstAttachmentAspectRatio) : Math.ceil(viewportWidth * aspectRatio)

    Accessible.description: {
        if (root.attachments.length === 0) {
            return "";
        }

        switch (root.attachments[0].attachmentType) {
            case Attachment.Image:
                return i18n("Status with image attachment");
            case Attachment.GifV:
                return i18n("Status with gif attachment");
            case Attachment.Video:
                return i18n("Status with video attachment");
            case Attachment.Audio:
                return i18n("Status with audio attachment");
        }
    }

    GridLayout {
        id: attachmentGridLayout

        anchors.fill: parent

        columns: Math.min(root.attachments.length, 2)
        rowSpacing: Kirigami.Units.smallSpacing
        columnSpacing: Kirigami.Units.smallSpacing

        Repeater {
            id: attachmentsRepeater
            model: root.secondary ? [] : root.attachments

            DelegateChooser {
                role: "attachmentType"

                DelegateChoice {
                    roleValue: Attachment.Image

                    MediaContainer {
                        id: imgContainer

                        required property var modelData
                        required property int index

                        caption: modelData.caption

                        onClicked: {
                            if (root.isSensitive) {
                                root.showMedia();
                            } else {
                                Navigation.openFullScreenImage(root.attachments, root.identity, imgContainer.index);
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive && img.status === Image.Ready) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        Accessible.description: root.isSensitive ? i18nc("@info", "Sensitive media") : modelData.caption

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.rowSpan: root.isSpecialAttachment(attachmentsRepeater.count, index) ? 2 : 1

                        FocusedImage {
                            id: img

                            anchors.fill: parent
                            source: imgContainer.modelData.previewUrl

                            onStatusChanged: {
                                if (status === Image.Error) {
                                    // Fall back to remote URL
                                    img.source = imgContainer.modelData.remoteUrl;
                                }
                            }

                            crop: !root.shouldKeepAspectRatio
                            focusX: imgContainer.modelData.focusX
                            focusY: imgContainer.modelData.focusY
                            sourceSize: Qt.size(imgContainer.modelData.sourceWidth, imgContainer.modelData.sourceHeight)

                            Rectangle {
                                anchors.fill: parent
                                color: "black"

                                Image {
                                    anchors.fill: parent

                                    source: visible ? imgContainer.modelData.tempSource : ''
                                }

                                visible: opacity !== 0.0
                                opacity: parent.status !== Image.Ready || root.isSensitive ? 1.0 : 0.0

                                Behavior on opacity {
                                    NumberAnimation {
                                        duration: Kirigami.Units.longDuration
                                    }
                                }
                            }

                            QQC2.BusyIndicator {
                                anchors.centerIn: parent
                                visible: parent.status !== Image.Ready
                            }

                            QQC2.Button {
                                anchors.centerIn: parent

                                visible: imgContainer.modelData.attachmentType === Attachment.Unknown
                                text: i18n("Not available")

                                onClicked: Qt.openUrlExternally(imgContainer.modelData.remoteUrl)
                            }
                        }
                    }
                }

                DelegateChoice {
                    roleValue: Attachment.GifV

                    VideoAttachment {
                        id: gif

                        required property var modelData
                        required property int index

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
                                gif.togglePlayPause()
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive && !gif.loading) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        onErrorOccurred: {
                            // Fall back to remote URL
                            gif.videoUrl = gif.modelData.remoteUrl;
                        }

                        Accessible.description: root.isSensitive ? i18nc("@info", "Sensitive media") : modelData.caption

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.rowSpan: root.isSpecialAttachment(attachmentsRepeater.count, index) ? 2 : 1

                        Connections {
                            target: root
                            function onInViewPortChanged() {
                                if (!root.inViewPort) {
                                    gif.pause();
                                } else if(gif.autoPlay) {
                                    gif.play();
                                }
                            }
                            function onUserSensitivityChanged(hide) {
                                if (hide) {
                                    gif.pause()
                                } else if (gif.autoPlay) {
                                    gif.play()
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
                        required property int index

                        videoUrl: modelData.source
                        previewUrl: modelData.previewUrl
                        autoPlay: false
                        isSensitive: root.isSensitive
                        looping: false
                        showVideoChip: true

                        Accessible.description: root.isSensitive ? i18nc("@info", "Sensitive media") : modelData.caption

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.rowSpan: root.isSpecialAttachment(attachmentsRepeater.count, index) ? 2 : 1

                        onClicked: {
                            if (root.isSensitive) {
                                root.showMedia();
                            } else {
                                video.togglePlayPause()
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive && !video.loading) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        onErrorOccurred: {
                            // Fall back to remote URL
                            video.videoUrl = video.modelData.remoteUrl;
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

                DelegateChoice {
                    roleValue: Attachment.Audio

                    AudioAttachment {
                        id: audio

                        required property var modelData
                        required property int index

                        audioUrl: modelData.source

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.rowSpan: root.isSpecialAttachment(attachmentsRepeater.count, index) ? 2 : 1

                        Accessible.description: root.isSensitive ? i18nc("@info", "Sensitive media") : modelData.caption

                        onClicked: {
                            if (root.isSensitive) {
                                root.showMedia();
                            } else {
                                audio.togglePlayPause()
                            }
                        }

                        onContextMenuRequested: {
                            if (!root.isSensitive && !audio.loading) {
                                root.openAttachmentMenu(modelData);
                            }
                        }

                        Connections {
                            target: root
                            function onInViewPortChanged() {
                                if (!root.inViewPort) {
                                    audio.pause();
                                }
                            }
                            function onUserSensitivityChanged(hide) {
                                audio.pause()
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

        QQC2.ToolTip.text: i18nc("@info:tooltip", "Hide media")
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        onClicked: root.hideMedia()
    }

    QQC2.Button {
        anchors.centerIn: parent

        visible: parent.isSensitive && parent.hasValidAttachment
        text: i18n("Show Media")
        icon.name: "view-visible-symbolic"

        onClicked: root.showMedia()
    }

    Loader {
        id: imageMenu

        active: false
        visible: active

        sourceComponent: AttachmentMenu {
            onClosed: imageMenu.active = false
        }
    }
}
