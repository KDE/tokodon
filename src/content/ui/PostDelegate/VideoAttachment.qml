// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import Qt.labs.qmlmodels 1.0

import QtQml 2.15

// Media container dedicated for videos and gifs
MediaContainer {
    id: root

    required property var videoUrl
    required property var previewUrl
    required property bool autoPlay
    required property bool isSensitive
    property alias showControls: mediaControls.visible
    property alias looping: player.looping

    function pause() {
        player.pause();
    }

    function play() {
        player.play();
    }

    function togglePlayPause() {
        if (player.paused) {
            player.play();
        } else {
            player.pause();
        }
    }

    MpvPlayer {
        id: player
        anchors.fill: parent

        autoPlay: root.autoPlay
        source: root.videoUrl
    }

    Image {
        anchors.fill: parent
        source: visible ? modelData.tempSource : ''
        visible: previewImage.status !== Image.Ready || root.isSensitive
    }

    Image {
        id: previewImage

        anchors.fill: parent
        source: root.previewUrl

        visible: (player.loading || player.stopped) && !root.isSensitive

        fillMode: Image.PreserveAspectCrop
    }

    HoverHandler {
        id: hoverHandler
    }

    QQC2.BusyIndicator {
        visible: player.loading && !root.isSensitive
        anchors.centerIn: parent
    }

    QQC2.Button {
        visible: {
            // don't overlay the controls with the "Media is hidden" message
            if (root.isSensitive) {
                return false;
            }

            // don't show the controls if the media is still loading
            if (player.loading) {
                return false;
            }

            // if the media is paused, definitely show them
            if (player.paused) {
                return true;
            }

            return false;
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

        visible: !root.isSensitive
        radius: previewImage.visible ? 0 : Kirigami.Units.smallSpacing
        color: Kirigami.Theme.backgroundColor
        opacity: {
            if (!player.paused) {
                return 0.7;
            }

            return hoverHandler.hovered || playPauseButton.hovered || videoSeekSlider.hovered ? 0.7 : 0.0
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

                icon.name: player.paused ? "media-playback-start" : "media-playback-pause"

                onClicked: root.togglePlayPause()
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

                onMoved: player.setPosition(value)
            }
        }
    }
}
