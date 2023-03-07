// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import QtGraphicalEffects 1.0
import Qt.labs.qmlmodels 1.0
import QtMultimedia 5.15

import QtQml 2.15

MediaContainer {
    id: root

    aspectRatio: output.sourceRect.height / output.sourceRect.width

    required property var videoUrl
    required property var previewUrl
    required property bool autoPlay
    required property bool isSensitive
    property alias showControls: mediaControls.visible

    function pause() {
        player.pause();
    }

    MediaPlayer {
        id: player

        autoPlay: root.autoPlay

        loops: MediaPlayer.Infinite

        source: root.videoUrl
        videoOutput: output

        function togglePlayPause() {
            if (playbackState === MediaPlayer.PlayingState) {
                pause();
            } else {
                play();
            }
        }

    }

    VideoOutput {
        id: output

        source: player
        fillMode: VideoOutput.PreserveAspectCrop
        flushMode: VideoOutput.FirstFrame

        anchors.fill: parent

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Item {
                width: output.width
                height: output.height
                Rectangle {
                    anchors.centerIn: parent
                    width: output.width
                    height: output.height
                    radius: Kirigami.Units.smallSpacing
                }
            }
        }
    }

    Image {
        anchors.fill: parent
        source: visible ? "image://blurhash/" + modelData.blurhash : ''
        visible: previewImage.status !== Image.Ready || root.isSensitive
    }

    Image {
        id: previewImage

        anchors.fill: parent
        source: root.previewUrl

        visible: player.status !== MediaPlayer.Buffered

        fillMode: Image.PreserveAspectCrop
    }

    MouseArea {
        id: playerMouseArea
        acceptedButtons: root.autoPlay ? Qt.NoButton : Qt.LeftButton
        hoverEnabled: true
        anchors.fill: parent

        onClicked: player.togglePlayPause()
    }

    Rectangle {
        id: mediaControls
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        height: mediaControlsLayout.implicitHeight

        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false

        radius: previewImage.visible ? 0 : Kirigami.Units.smallSpacing
        color: Kirigami.Theme.backgroundColor
        opacity: {
            if (player.playbackState !== MediaPlayer.PlayingState) {
                return 0.7;
            }

            return playerMouseArea.containsMouse || playPauseButton.hovered || videoSeekSlider.hovered ? 0.7 : 0.0
        }
        Behavior on opacity {
            OpacityAnimator {
                duration: Kirigami.Units.longDuration
            }
        }

        RowLayout {
            id: mediaControlsLayout
            anchors.fill: parent

            QQC2.ToolButton {
                id: playPauseButton
                Layout.alignment: Qt.AlignVCenter

                icon.name: {
                    switch (player.playbackState) {
                    case MediaPlayer.PlayingState:
                        return "media-playback-pause";
                    case MediaPlayer.PausedState:
                    case MediaPlayer.StoppedState:
                        return "media-playback-start";
                    }
                }

                onClicked: player.togglePlayPause()
            }

            QQC2.Slider {
                id: videoSeekSlider
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true

                from: 0
                to: player.duration

                Binding {
                    target: videoSeekSlider
                    property: "value"
                    value: player.position
                    when: !videoSeekSlider.pressed
                    restoreMode: Binding.RestoreBindingOrValue
                }

                onMoved: player.seek(value)
            }
        }
    }
}
