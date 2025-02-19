// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

import QtQml 2.15

// Media container dedicated for videos and gifs
MediaContainer {
    id: root

    required property var videoUrl
    required property var previewUrl
    required property bool autoPlay
    required property bool isSensitive
    property alias showControls: mediaControls.visible
    property bool looping: false
    property alias loading: busyIndicator.visible

    signal errorOccurred(error: int, errorString: string)

    function pause() {
        // Unlike the other functions, we want to make sure the item doesn't accidentally get created when the video is auto-paused due to scrolling
        if (player.active) {
            player.item?.pause();
        }
    }

    function play() {
        player.active = true;
        player.item?.play();
    }

    function togglePlayPause() {
        player.active = true;
        if (player.item?.paused || player.item?.stopped) {
            player.item?.play();
        } else {
            player.item?.pause();
        }
    }

    Loader {
        id: player
        active: root.autoPlay

        anchors.fill: parent

        sourceComponent: VideoPlayer {
            autoPlay: root.autoPlay
            source: root.videoUrl
            looping: root.looping
            onErrorOccurred: (error, errorString) => root.errorOccurred(error, errorString)
        }
    }

    Image {
        anchors.fill: parent
        source: visible ? modelData.tempSource : ''
        visible: previewImage.visible && (previewImage.status !== Image.Ready || root.isSensitive)
    }

    Image {
        id: previewImage

        anchors.fill: parent
        source: root.previewUrl

        visible: ((player.item?.loading ?? false) || (player.item?.stopped ?? true)) && !root.isSensitive

        fillMode: Image.PreserveAspectCrop
    }

    HoverHandler {
        id: hoverHandler
    }

    QQC2.BusyIndicator {
        id: busyIndicator
        visible: player.active && (player.item?.loading ?? false) && !root.isSensitive
        anchors.centerIn: parent
    }

    QQC2.Button {
        visible: {
            // don't overlay the controls with the "Media is hidden" message
            if (root.isSensitive) {
                return false;
            }

            if (!player.active) {
                return true;
            }

            // don't show the controls if the media is still loading
            if (player.item.loading) {
                return false;
            }

            // if the media is paused, definitely show them
            return player.item?.paused || player.item?.stopped;
        }
        anchors.centerIn: parent
        onClicked: root.play()

        Accessible.name: i18nc("@action:button Start media playback", "Play")

        icon {
            name: "media-playback-start"
            width: Kirigami.Units.iconSizes.large
            height: Kirigami.Units.iconSizes.large
        }
    }

    Rectangle {
        id: mediaControls
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: Kirigami.Units.largeSpacing
        }
        height: mediaControlsLayout.implicitHeight

        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false

        radius: Kirigami.Units.cornerRadius
        color: Kirigami.Theme.backgroundColor
        opacity: hoverHandler.hovered && !root.isSensitive && !(player.item?.paused ?? true) && !(player.item?.stopped ?? true) ? 0.7 : 0.0
        Behavior on opacity {
            OpacityAnimator {
                duration: Kirigami.Units.longDuration
            }
        }

        RowLayout {
            id: mediaControlsLayout
            anchors.fill: parent

            spacing: Kirigami.Units.smallSpacing

            QQC2.ToolButton {
                id: playPauseButton
                Layout.alignment: Qt.AlignVCenter

                icon.name: "media-playback-pause"

                onClicked: root.togglePlayPause()
            }

            QQC2.Slider {
                id: videoSeekSlider
                Layout.alignment: Qt.AlignVCenter
                Layout.rightMargin: Kirigami.Units.smallSpacing
                Layout.fillWidth: true

                from: 0
                to: player.item?.duration ?? 0.0

                Binding {
                    target: videoSeekSlider
                    property: "value"
                    value: player.item?.position
                    when: !videoSeekSlider.pressed
                    restoreMode: Binding.RestoreBindingOrValue
                }

                onMoved: player.item?.setPosition(value)
            }
        }
    }
}
