// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.tokodon

// Media container dedicated for videos and gifs
MediaContainer {
    id: root

    required property var audioUrl
    readonly property bool loading: player.mediaStatus === MediaPlayer.LoadingMedia

    function pause() {
        // Unlike the other functions, we want to make sure the item doesn't accidentally get created when the video is auto-paused due to scrolling
        player.pause();
    }

    function play() {
        player.play();
    }

    function togglePlayPause() {
        if (player.playbackState === MediaPlayer.PausedState || player.playbackState === MediaPlayer.StoppedState) {
            player.play();
        } else {
            player.pause();
        }
    }

    MediaPlayer {
        id: player
        source: root.audioUrl
        audioOutput: AudioOutput {}
    }

    Rectangle {
        anchors.fill: parent

        color: Kirigami.Theme.alternateBackgroundColor

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        Kirigami.Icon {
            anchors.centerIn: parent

            source: "library-music-symbolic"
            implicitWidth: Kirigami.Units.iconSizes.huge
            implicitHeight: Kirigami.Units.iconSizes.huge
        }
    }

    HoverHandler {
        id: hoverHandler
    }

    QQC2.BusyIndicator {
        visible: player.mediaStatus === MediaPlayer.LoadingMedia
        anchors.centerIn: parent
    }

    QQC2.Button {
        visible: {
            // don't show the controls if the media is still loading
            if (player.mediaStatus === MediaPlayer.LoadingMedia) {
                return false;
            }

            // if the media is paused, definitely show them
            return player.playbackState === MediaPlayer.PausedState || player.playbackState === MediaPlayer.StoppedState;
        }
        anchors.centerIn: parent
        onClicked: player.play()

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
        opacity: hoverHandler.hovered && player.playbackState !== MediaPlayer.PausedState && player.playbackState !== MediaPlayer.StoppedState ? 0.7 : 0.0
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
                to: player.duration ?? 0.0

                Binding {
                    target: videoSeekSlider
                    property: "value"
                    value: player.position
                    when: !videoSeekSlider.pressed
                    restoreMode: Binding.RestoreBindingOrValue
                }

                onMoved: player.setPosition(value)
            }
        }
    }
}
