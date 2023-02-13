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

MediaContainer {
    id: root

    aspectRatio: output.sourceRect.height / output.sourceRect.width

    required property var videoUrl
    required property var previewUrl
    required property bool autoPlay
    required property bool isSensitive

    MediaPlayer {
        id: player

        autoPlay: root.autoPlay

        loops: MediaPlayer.Infinite

        source: root.videoUrl
        videoOutput: output
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

        visible: player.playbackState !== MediaPlayer.PlayingState

        fillMode: Image.PreserveAspectCrop
    }

    QQC2.Button {
        visible: player.playbackState !== MediaPlayer.PlayingState
        anchors.centerIn: parent
        onClicked: player.play()
        text: i18n("Play")
        icon {
            width: Kirigami.Units.iconSizes.large
            height: Kirigami.Units.iconSizes.large
        }
    }
}