// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.kirigamiaddons.labs.components as Labs
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

Kirigami.Page {
    id: accountInfo

    required property string accountId

    property var postsBar
    property string selectedTag

    property var currentIndex: 0
    readonly property bool onPostsTab: currentIndex === 0
    readonly property bool onRepliesTab: currentIndex === 1
    readonly property bool onMediaTab: currentIndex === 2

    readonly property bool canExcludeBoosts: onPostsTab || onRepliesTab
    property alias excludeBoosts: accountModel.excludeBoosts

    readonly property bool largeScreen: width > Kirigami.Units.gridUnit * 25

    title: accountModel.identity.displayName
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight

        text: accountModel.identity.displayNameHtml

        textFormat: Text.StyledText
    }

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        color: Kirigami.Theme.backgroundColor
    }

    Connections {
        target: accountInfo.postsBar
        enabled: accountInfo.postsBar !== null
        ignoreUnknownSignals: true // postsBar is null when this is initially constructed

        function onCurrentIndexChanged(): void {
            accountInfo.currentIndex = accountInfo.postsBar.currentIndex;
            stackLayout.currentIndex = accountInfo.currentIndex === 2 ? 1 : 0;
        }
    }

    function updateTabs(): void {
        if (stackLayout.currentIndex === 0) {
            postsBar = stackLayout.children[stackLayout.currentIndex].contentItem.headerItem.getBar();
        } else if (stackLayout.currentIndex === 1) {
            mediaTabLoader.active = true;
            postsBar = stackLayout.children[stackLayout.currentIndex].item?.contentItem.headerItem.getBar();
        }
        postsBar.currentIndex = accountInfo.currentIndex;
    }

    StackLayout {
        id: stackLayout

        anchors.fill: parent

        implicitHeight: children[currentIndex].implicitHeight

        onCurrentIndexChanged: accountInfo.updateTabs()

        Component.onCompleted: accountInfo.updateTabs()

        QQC2.ScrollView {
            focus: true
            clip: true

            Keys.onPressed: event => timelineView.handleKeyEvent(event)

            TimelineView {
                id: timelineView

                Kirigami.Theme.colorSet: Kirigami.Theme.View

                header: AccountHeader {
                    identity: accountModel.identity
                    isSelf: accountModel.isSelf
                    width: timelineView.width
                }

                model: AccountModel {
                    id: accountModel

                    accountId: accountInfo.accountId
                    tagged: accountInfo.selectedTag
                    currentTab: accountInfo.currentIndex
                }

                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    text: i18nc("@label", "No Posts")
                    explanation: i18n("This user hasn't posted anything yet.")
                    visible: timelineView.count === 0 && !timelineView.model.loading
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }

        Loader {
            id: mediaTabLoader

            active: false

            sourceComponent: QQC2.ScrollView {
                clip: true

                GridView {
                    id: gridView

                    property int numCells: gridView.width < 1000 ? 3 : 5
                    property real cellSize: gridView.width / numCells

                    header: AccountHeader {
                        identity: accountModel.identity
                        isSelf: accountModel.isSelf
                        width: gridView.width
                    }
                    cellWidth: cellSize
                    cellHeight: cellSize

                    model: AccountMediaTimelineModel {
                        accountId: accountInfo.accountId
                        tagged: accountInfo.selectedTag
                    }

                    delegate: Item {
                        id: imageDelegate

                        required property string postId
                        required property string source
                        required property url tempSource
                        required property real focusX
                        required property real focusY
                        required property bool sensitive
                        required property var attachment

                        width: gridView.cellWidth
                        height: gridView.cellHeight

                        FocusedImage {
                            id: image

                            anchors.fill: parent

                            source: imageDelegate.source
                            focusX: imageDelegate.focusX
                            focusY: imageDelegate.focusY
                        }

                        Image {
                            id: tempImage

                            anchors.fill: parent

                            source: imageDelegate.tempSource
                            visible: image.status !== Image.Ready || imageDelegate.sensitive

                            Kirigami.Icon {
                                anchors.centerIn: parent
                                source: "view-hidden-symbolic"
                            }
                        }

                        Rectangle {
                            anchors.fill: parent

                            visible: hoverHandler.hovered
                            color: "transparent"

                            border {
                                width: 2
                                color: Kirigami.Theme.hoverColor
                            }
                        }

                        TapHandler {
                            onTapped: Navigation.openFullScreenImage([imageDelegate.attachment], accountModel.identity, 0);
                        }

                        HoverHandler {
                            id: hoverHandler
                            acceptedDevices: PointerDevice.AllDevices
                        }
                    }

                    Kirigami.PlaceholderMessage {
                        anchors.centerIn: parent
                        text: i18nc("@label", "No Media")
                        explanation: i18n("This user hasn't posted any media yet.")
                        visible: gridView.count === 0 && !gridView.model.loading
                        width: parent.width - Kirigami.Units.gridUnit * 4
                    }
                }
            }
        }
    }
}
