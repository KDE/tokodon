// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.tokodon
import org.kde.tokodon.private
import '..'
import '../PostDelegate'

ListView {
    id: root

    property bool expandedPost: false
    property bool completedInitialLoad: false

    reuseItems: false // TODO: this causes jumping on the timeline. needs more investigation before it's re-enabled

    Connections {
        target: root.model
        function onPostSourceReady(backend, isEdit): void {
            const item = pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: isEdit ? StatusComposer.Edit : StatusComposer.Redraft,
                backend: backend
            });
            item.refreshData(); // to refresh spoiler text, etc
        }

        function onLoadingChanged(): void {
            if (!root.model.loading) {
                root.completedInitialLoad = true;
            }
        }

        function onRepositionAt(index): void {
            root.positionViewAtIndex(index, ListView.Beginning);
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

        visible: root.model.loading && !root.completedInitialLoad

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
            id: endOfTimelineMessage

            visible: root.model.atEnd ?? false
            text: i18nc("@info:status", "End of Timeline")

            Layout.topMargin: Kirigami.Units.largeSpacing
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
            target: ListView.view

            function onContentYChanged(): void {
                const aMin = status.y;
                const aMax = status.y + status.height;

                const bMin = ListView.view.contentY;
                const bMax = ListView.view.contentY + ListView.view.height;

                if (!root.isCurrentPage) {
                    status.inViewPort = false;
                    return;
                }

                let topEdgeVisible;
                let bottomEdgeVisible;

                // we are still checking two rectangles, but if one is bigger than the other
                // just switch which one should be checked.
                if (status.height > ListView.view.height) {
                    topEdgeVisible = bMin > aMin && bMin < aMax;
                    bottomEdgeVisible = bMax > aMin && bMax < aMax;
                } else {
                    topEdgeVisible = aMin > bMin && aMin < bMax;
                    bottomEdgeVisible = aMax > bMin && aMax < bMax;
                }

                status.inViewPort = topEdgeVisible || bottomEdgeVisible;
            }
        }

        Connections {
            target: applicationWindow()

            function onIsShowingFullScreenImageChanged(): void {
                if (applicationWindow().isShowingFullScreenImage) {
                    status.inViewPort = false;
                } else {
                    ListView.view.contentYChanged();
                }
            }
        }
    }
}