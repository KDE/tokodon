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
    property real volume: 1.0

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
            volume: root.volume
        }
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

        visible: ((player.item?.loading ?? false) || (player.item?.stopped ?? true)) && !root.isSensitive

        fillMode: Image.PreserveAspectCrop
    }

    HoverHandler {
        id: hoverHandler

        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onHoveredChanged: {
            if (hovered) {
                mediaControlsHideTimer.restart();
            }
        }
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
        opacity: (mediaControlsHideTimer.running || volumePopupTimer || hoverHandler.hovered) && !root.isSensitive && !(player.item?.paused ?? true) && !(player.item?.stopped ?? true) ? 0.7 : 0.0
        Behavior on opacity {
            OpacityAnimator {
                duration: Kirigami.Units.longDuration
            }
        }

        Timer {
            id: mediaControlsHideTimer
            interval: 5000
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

            QQC2.ToolButton {
                id: volumeButton

                property var unmuteVolume: root.volume

                icon.name: root.volume <= 0 ? "player-volume-muted-symbolic" : "player-volume-symbolic"

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.timeout: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.text: i18nc("@action:button", "Volume")

                onClicked: {
                    if (root.volume > 0) {
                        root.volume = 0;
                    } else  if (unmuteVolume === 0) {
                        root.volume = 1;
                    } else {
                        root.volume = unmuteVolume;
                    }
                }

                QQC2.Popup {
                    id: volumePopup
                    y: -height
                    width: volumeButton.width
                    visible: (volumeButton.hovered || volumePopupHoverHandler.hovered || volumeSlider.hovered || volumePopupTimer.running)

                    focus: true
                    padding: Kirigami.Units.smallSpacing
                    closePolicy: QQC2.Popup.NoAutoClose

                    QQC2.Slider {
                        id: volumeSlider
                        anchors.centerIn: parent
                        implicitHeight: Kirigami.Units.gridUnit * 7
                        orientation: Qt.Vertical
                        padding: 0
                        from: 0
                        to: 1
                        value: root.volume
                        onMoved: {
                            root.volume = value;
                            volumeButton.unmuteVolume = value;
                        }
                    }
                    Timer {
                        id: volumePopupTimer
                        interval: 1000
                    }
                    HoverHandler {
                        id: volumePopupHoverHandler

                        onHoveredChanged: {
                            if (hovered) {
                                volumePopupTimer.restart();
                            }
                        }
                    }
                    background: Kirigami.ShadowedRectangle {
                        radius: Kirigami.Units.cornerRadius
                        color: Kirigami.Theme.backgroundColor
                        opacity: 0.8

                        shadow {
                            xOffset: 0
                            yOffset: 4
                            color: Qt.rgba(0, 0, 0, 0.3)
                            size: Kirigami.Units.largeSpacing
                       }
                    }
                }
            }
        }
    }
}
