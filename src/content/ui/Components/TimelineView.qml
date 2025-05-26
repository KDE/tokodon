// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.tokodon

import '..'
import '../PostDelegate'

ListView {
    id: root

    // Set to expand all of the posts in the list
    property bool expandedPost: false

    // Set to the original post URL to show the "show more replies" message
    property string originalPostUrl

    // This causes jumping on the timeline. needs more investigation before it's re-enabled
    reuseItems: false

    // Call this function in Keys.onPressed to get PgUp/PgDn support
    function handleKeyEvent(event): void {
        if (event.key === Qt.Key_PageUp && !root.atYBeginning) {
            event.accepted = true;
            root.contentY -= height;
        } else if (event.key === Qt.Key_PageDown && !root.atYEnd) {
            event.accepted = true;
            root.contentY += height;
        }
        root.contentY = Math.min(Math.max(root.contentY, 0), root.contentHeight);
    }

    // Used for pages like TimelinePage to control video playback
    property bool isCurrentPage: true

    Connections {
        target: root.model
        function onPostSourceReady(backend, isEdit): void {
            const item = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                purpose: isEdit ? StatusComposer.Edit : StatusComposer.Redraft,
                backend: backend
            });
            item.refreshData(); // to refresh spoiler text, etc
        }

        function onRepositionAt(index): void {
            root.positionViewAtIndex(index, ListView.Beginning);
        }

        function onStreamedPostAdded(id: string): void {
            // Update the read marker if we're at the top and a post just came in
            if (root.atYBeginning && root.model.updateReadMarker) {
                root.model.updateReadMarker(id);
            }
        }
    }

    Components.FloatingButton {
        QQC2.ToolTip.text: i18nc("@info:tooltip", "Return to Top")
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        opacity: root.atYBeginning ? 0 : 1
        visible: opacity !== 0

        Behavior on opacity {
            NumberAnimation {}
        }

        anchors {
            right: parent.right
            rightMargin: Kirigami.Units.largeSpacing
            bottom: parent.bottom
            bottomMargin: Kirigami.Units.largeSpacing
        }

        action: Kirigami.Action {
            icon.name: "arrow-up"
            onTriggered: root.positionViewAtBeginning()
        }
    }

    Rectangle {
        anchors {
            fill: parent
            topMargin: root.headerItem ? root.headerItem.height : 0
        }

        visible: root.model.loading && root.count === 0

        color: Kirigami.Theme.backgroundColor

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
        }
    }

    footer: Kirigami.FlexColumn {
        id: flexColumn

        spacing: Kirigami.Units.largeSpacing

        padding: 0
        maximumWidth: Kirigami.Units.gridUnit * 40

        width: parent.width
        implicitHeight: Kirigami.Units.gridUnit * 4
        visible: ListView.view.count > 0

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: endOfTimelineMessage.visible || loadingBar.visible
        }

        Kirigami.PlaceholderMessage {
            id: repliesNotAvailableMessage

            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            Layout.fillHeight: true

            visible: root.originalPostUrl.length !== 0
            text: i18nc("@info:status", "Some replies are not available")
            explanation: i18n("To view all replies, open the post on the original server.")
            helpfulAction: Kirigami.Action {
                icon.name: "open-link-symbolic"
                text: i18nc("@action:button 'Browser' being a web browser", "Open in Browser")
                onTriggered: Qt.openUrlExternally(root.originalPostUrl)
            }
        }

        Kirigami.PlaceholderMessage {
            id: endOfTimelineMessage

            visible: root.model.atEnd ?? false
            text: i18nc("@info:status", "End of Timeline")

            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing * 2
            Layout.alignment: Qt.AlignHCenter
        }

        QQC2.ProgressBar {
            id: loadingBar

            visible: root.model.loading
            indeterminate: true

            Layout.alignment: Qt.AlignHCenter
        }
    }

    delegate: PostDelegate {
        id: status

        timelineModel: ListView.view.model
        expandedPost: root.expandedPost
        showSeparator: index !== ListView.view.count - 1
        loading: ListView.view.model.loading
        width: ListView.view.width

        Connections {
            target: status.ListView.view

            function onContentYChanged(): void {
                const aMin = status.y;
                const aMax = status.y + status.height;

                const bMin = status.ListView.view.contentY;
                const bMax = status.ListView.view.contentY + status.ListView.view.height;

                if (!root.isCurrentPage) {
                    status.inViewPort = false;
                    return;
                }

                let topEdgeVisible;
                let bottomEdgeVisible;

                // we are still checking two rectangles, but if one is bigger than the other
                // just switch which one should be checked.
                if (status.height > status.ListView.view.height) {
                    topEdgeVisible = bMin > aMin && bMin < aMax;
                    bottomEdgeVisible = bMax > aMin && bMax < aMax;
                } else {
                    topEdgeVisible = aMin > bMin && aMin < bMax;
                    bottomEdgeVisible = aMax > bMin && aMax < bMax;
                }

                status.inViewPort = topEdgeVisible || bottomEdgeVisible;
                if (status.inViewPort && status.ListView.view.model.updateReadMarker) {
                    status.ListView.view.model.updateReadMarker(status.originalId);
                }
            }
        }

        Connections {
            target: root

            function onIsCurrentPageChanged() {
                if (!root.isCurrentPage) {
                    status.inViewPort = false;
                } else {
                    status.ListView.view.contentYChanged();
                }
            }
        }

        Connections {
            target: applicationWindow()

            function onIsShowingFullScreenImageChanged(): void {
                if (applicationWindow().isShowingFullScreenImage) {
                    status.inViewPort = false;
                } else {
                    status.ListView.view.contentYChanged();
                }
            }
        }
    }
}
