// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private
import Qt5Compat.GraphicalEffects
import Qt.labs.qmlmodels 1.0

import "../Components"

QQC2.Control {
    id: root

    required property var attachments
    required property var identity
    required property bool sensitive
    required property bool secondary
    required property bool expandedPost
    required property bool inViewPort

    property bool canHideMedia: true

    // Only uncrop timeline media if requested by the user, and there's only one attachment
    // Expanded posts (like in threads) are always uncropped.
    readonly property var shouldKeepAspectRatio: (!Config.cropMedia || root.expandedPost) && root.attachments.length === 1

    property bool isSensitive: (AccountManager.selectedAccount.preferences.extendMedia === "hide_all" ? true : (AccountManager.selectedAccount.preferences.extendMedia === "show_all" ? false : root.sensitive))
    signal userSensitivityChanged(hide: bool)

    property bool hasValidAttachment: {
        for (let i in root.attachments) {
            if (root.attachments[i].attachmentType !== Attachment.Unknown) {
                return true;
            }
        }
        return false;
    }

    Layout.fillWidth: true
    Layout.fillHeight: shouldKeepAspectRatio
    Layout.topMargin: Kirigami.Units.largeSpacing

    topPadding: 0
    leftPadding: 0
    bottomPadding: 0
    rightPadding: 0

    contentItem: GridLayout {
        id: attachmentGridLayout
        columns: Math.min(root.attachments.length, 2)

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
                        rootWidth: root.width
                        gridLayout: attachmentGridLayout

                        sourceWidth: modelData.sourceWidth > img.sourceSize.width ? modelData.sourceWidth : img.sourceSize.width
                        sourceHeight: modelData.sourceHeight > img.sourceSize.height ? modelData.sourceHeight : img.sourceSize.height

                        FocusedImage {
                            id: img

                            anchors.fill: parent
                            source: modelData.previewUrl

                            crop: !root.shouldKeepAspectRatio
                            focusX: modelData.focusX
                            focusY: modelData.focusY

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

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (root.isSensitive) {
                                        root.isSensitive = false;
                                        root.userSensitivityChanged(false);
                                    } else {
                                        Navigation.openFullScreenImage(root.attachments, root.identity, imgContainer.index);
                                    }
                                }
                            }

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
                                visible: modelData.attachmentType === Attachment.Unknown
                                text: i18n("Not available")
                                anchors.centerIn: parent
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
                        rootWidth: root.width
                        gridLayout: attachmentGridLayout

                        videoUrl: modelData.source
                        previewUrl: modelData.previewUrl
                        autoPlay: Config.autoPlayGif && !root.isSensitive
                        isSensitive: root.isSensitive
                        showControls: false
                        looping: true

                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (root.isSensitive) {
                                root.isSensitive = false;
                                root.userSensitivityChanged(false);
                            } else {
                                video.togglePlayPause()
                            }
                        }

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

                        Kirigami.Chip {
                            anchors {
                                top: parent.top
                                topMargin: Kirigami.Units.largeSpacing
                                right: parent.right
                                rightMargin: Kirigami.Units.largeSpacing
                            }

                            checked: false
                            checkable: false
                            text: i18n("GIF")
                            closable: false
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
                        rootWidth: root.width
                        gridLayout: attachmentGridLayout

                        videoUrl: modelData.source
                        previewUrl: modelData.previewUrl
                        autoPlay: false
                        isSensitive: root.isSensitive
                        looping: false

                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (root.isSensitive) {
                                root.isSensitive = false;
                                root.userSensitivityChanged(false);
                            } else {
                                video.togglePlayPause()
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

                        Kirigami.Chip {
                            anchors {
                                top: parent.top
                                topMargin: Kirigami.Units.largeSpacing
                                right: parent.right
                                rightMargin: Kirigami.Units.largeSpacing
                            }

                            checked: false
                            checkable: false
                            text: i18n("Video")
                            closable: false
                        }
                    }
                }
            }
        }
    }

    QQC2.Button {
        icon.name: "view-hidden"

        visible: !parent.isSensitive && parent.hasValidAttachment && root.canHideMedia

        anchors.top: parent.top
        anchors.topMargin: Kirigami.Units.smallSpacing
        anchors.left: parent.left
        anchors.leftMargin: Kirigami.Units.smallSpacing

        onClicked: {
            root.isSensitive = true
            root.userSensitivityChanged(true)
        }
    }

    QQC2.Button {
        anchors.centerIn: parent

        visible: parent.isSensitive && parent.hasValidAttachment

        text: i18n("Media Hidden")
        onClicked: if (root.isSensitive) {
            root.isSensitive = false;
            root.userSensitivityChanged(false);
        } else {
            Navigation.openFullScreenImage(root.attachments, root.identity, 0);
        }
    }
}
