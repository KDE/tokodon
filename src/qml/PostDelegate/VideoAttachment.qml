// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon

import QtQml

// Media container dedicated for videos and gifs
MediaContainer {
    id: root

    required property string videoUrl
    required property string previewUrl
    property url tempSource
    required property bool autoPlay
    required property bool isSensitive
    property alias showControls: mediaControls.visible
    property bool looping: false
    property alias loading: busyIndicator.visible
    property real volume: 1.0

    /// The loaded player, or null until playback has been requested
    readonly property VideoPlayer videoPlayer: player.item as VideoPlayer

    signal errorOccurred(error: int, errorString: string)

    function pause() {
        // Unlike the other functions, we want to make sure the item doesn't accidentally get created when the video is auto-paused due to scrolling
        if (player.active) {
            root.videoPlayer?.pause();
        }
    }

    function play() {
        player.active = true;
        root.videoPlayer?.play();
    }

    function togglePlayPause() {
        player.active = true;
        if (root.videoPlayer?.paused || root.videoPlayer?.stopped) {
            root.videoPlayer?.play();
        } else {
            root.videoPlayer?.pause();
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
        source: visible ? root.tempSource : ''
        visible: previewImage.status !== Image.Ready || root.isSensitive
    }

    Image {
        id: previewImage

        anchors.fill: parent
        source: root.previewUrl

        visible: ((root.videoPlayer?.loading ?? false) || (root.videoPlayer?.stopped ?? true)) && !root.isSensitive

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
        visible: player.active && (root.videoPlayer?.loading ?? false) && !root.isSensitive
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
            if (root.videoPlayer?.loading) {
                return false;
            }

            // if the media is paused, definitely show them
            return root.videoPlayer?.paused || root.videoPlayer?.stopped;
        }
        anchors.centerIn: parent
        onClicked: root.play()

        Accessible.name: KI18n.i18nc("@action:button Start media playback", "Play")

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
        opacity: (mediaControlsHideTimer.running || volumePopupTimer || hoverHandler.hovered) && !root.isSensitive && !(root.videoPlayer?.paused ?? true) && !(root.videoPlayer?.stopped ?? true) ? 0.7 : 0.0
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
                to: root.videoPlayer?.duration ?? 0.0

                Binding {
                    target: videoSeekSlider
                    property: "value"
                    value: root.videoPlayer?.position
                    when: !videoSeekSlider.pressed
                    restoreMode: Binding.RestoreBindingOrValue
                }

                onMoved: root.videoPlayer?.setPosition(value)
            }

            QQC2.ToolButton {
                id: volumeButton

                property real unmuteVolume: root.volume

                icon.name: root.volume <= 0 ? "player-volume-muted-symbolic" : "player-volume-symbolic"

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.timeout: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.text: KI18n.i18nc("@action:button", "Volume")

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
