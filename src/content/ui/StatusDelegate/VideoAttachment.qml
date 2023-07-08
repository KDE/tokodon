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

import QtQml 2.15

MediaContainer {
    id: root

    sourceWidth: Math.max(modelData.sourceWidth, player.sourceSize.width)
    sourceHeight: Math.max(modelData.sourceHeight, player.sourceSize.height)

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

    QQC2.BusyIndicator {
        visible: player.loading && !root.isSensitive
        anchors.centerIn: parent
    }

    MouseArea {
        id: playerMouseArea
        acceptedButtons: root.autoPlay ? Qt.NoButton : Qt.LeftButton
        hoverEnabled: true
        anchors.fill: parent

        onClicked: root.togglePlayPause()
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
