// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtMultimedia

Item {
    id: root

    readonly property bool paused: player.playbackState === MediaPlayer.PausedState
    readonly property bool stopped: player.playbackState === MediaPlayer.StoppedState
    readonly property bool loading: player.mediaStatus === MediaPlayer.LoadingMedia
    readonly property size sourceSize: Qt.size(videoOutput.sourceRect.width, videoOutput.sourceRect.height)

    property alias duration: player.duration
    property bool autoPlay
    property alias source: player.source
    property alias position: player.position
    property bool looping
    property alias volume: audioOutput.volume

    signal errorOccurred(error: int, errorString: string)

    function play(): void {
        player.play();
    }

    function pause(): void {
        player.pause();
    }

    function setPosition(pos: int): void {
        player.position = pos;
    }

    MediaPlayer {
        id: player
        videoOutput: videoOutput
        audioOutput: AudioOutput {
            id: audioOutput
        }
        loops: root.looping ? MediaPlayer.Infinite : 0

        onMediaStatusChanged: {
            if (player.mediaStatus !== MediaPlayer.LoadingMedia) {
                if (root.autoPlay) {
                    player.play();
                }
            }
        }

        onErrorOccurred: (error, errorString) => root.errorOccurred(error, errorString)
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
    }
}
