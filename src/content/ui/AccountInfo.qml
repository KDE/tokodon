// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.tokodon

Kirigami.Page {
    id: accountInfo

    required property string accountId

    property string pageId
    property var postsBar
    property string selectedTag

    property var currentIndex: 0
    readonly property bool onPostsTab: currentIndex === 0
    readonly property bool onRepliesTab: currentIndex === 1
    readonly property bool onMediaTab: currentIndex === 2

    readonly property bool canExcludeBoosts: onPostsTab || onRepliesTab
    property alias excludeBoosts: accountModel.excludeBoosts

    readonly property bool largeScreen: width > Kirigami.Units.gridUnit * 25

    title: accountModel.identity ? accountModel.identity.displayName : i18nc("@title:window", "Profile")
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight

        text: accountModel.identity ? accountModel.identity.displayNameHtml : i18nc("@title:window", "Profile")

        textFormat: Text.StyledText
    }

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    actions: Kirigami.Action {
        icon.name: "list-add"
        text: i18nc("@action:button", "Post")
        onTriggered: Navigation.openComposer("")
        visible: accountModel.isSelf
    }

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        color: Kirigami.Theme.backgroundColor
    }

    Connections {
        target: accountInfo.postsBar ?? null
        enabled: accountInfo.postsBar !== null
        ignoreUnknownSignals: true // postsBar is null when this is initially constructed

        function onCurrentIndexChanged(): void {
            accountInfo.currentIndex = accountInfo.postsBar.currentIndex;
            stackLayout.currentIndex = accountInfo.currentIndex === 2 ? 1 : 0;
        }
    }

    function updateTabs(): void {
        if (stackLayout.currentIndex === 0) {
            postsBar = stackLayout.children[stackLayout.currentIndex].contentItem.headerItem.item?.getBar();
        } else if (stackLayout.currentIndex === 1) {
            mediaTabLoader.active = true;
            postsBar = stackLayout.children[stackLayout.currentIndex].item?.contentItem.headerItem.getBar();
        }
        if (postsBar) {
            postsBar.currentIndex = accountInfo.currentIndex;
        }
    }

    StackLayout {
        id: stackLayout

        anchors.fill: parent

        implicitHeight: children[currentIndex].implicitHeight

        onCurrentIndexChanged: accountInfo.updateTabs()

        QQC2.ScrollView {
            focus: true
            clip: true

            Keys.onPressed: event => timelineView.handleKeyEvent(event)

            TimelineView {
                id: timelineView

                Kirigami.Theme.colorSet: Kirigami.Theme.View

                header: Loader {
                    active: accountModel.identity
                    onLoaded: accountInfo.updateTabs()

                    sourceComponent: AccountHeader {
                        identity: accountModel.identity
                        isSelf: accountModel.isSelf
                        width: timelineView.width
                        largeScreen: accountInfo.largeScreen
                        canExcludeBoosts: accountInfo.canExcludeBoosts
                        excludeBoosts: accountInfo.excludeBoosts
                        onExcludeBoostsToggled: (checked) => accountInfo.excludeBoosts = checked
                        onSelectedTagChanged: (selectedTag) => accountInfo.selectedTag = selectedTag
                    }
                }

                model: AccountModel {
                    id: accountModel

                    accountId: accountInfo.accountId
                    tagged: accountInfo.selectedTag
                    currentTab: accountInfo.currentIndex
                }

                Kirigami.PlaceholderMessage {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        top: parent.top
                        // Take into account the header item, which the center anchor doesn't do
                        topMargin: parent.headerItem.height + ((parent.height - parent.headerItem.height) / 2)
                    }
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
                        largeScreen: accountInfo.largeScreen
                        canExcludeBoosts: accountInfo.canExcludeBoosts
                        excludeBoosts: accountInfo.excludeBoosts
                        onExcludeBoostsToggled: (checked) => accountInfo.excludeBoosts = checked
                        onSelectedTagChanged: (selectedTag) => accountInfo.selectedTag = selectedTag
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
                            visible: opacity !== 0.0
                            opacity: image.status !== Image.Ready || imageDelegate.sensitive ? 1.0 : 0.0

                            Kirigami.Icon {
                                anchors.centerIn: parent
                                source: "view-hidden-symbolic"
                                visible: imageDelegate.sensitive
                            }

                            QQC2.BusyIndicator {
                                id: busyIndicator
                                visible: !imageDelegate.sensitive
                                anchors.centerIn: parent
                            }

                            Behavior on opacity {
                                NumberAnimation {
                                    duration: Kirigami.Units.longDuration
                                }
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
                            onTapped: {
                                const dialog = fullScreenImage.createObject(QQC2.Overlay.overlay, {
                                    attachments: [imageDelegate.attachment],
                                    identity: accountModel.identity,
                                    initialIndex: 0,
                                    postId: imageDelegate.postId
                                });
                                dialog.open();
                            }
                        }

                        HoverHandler {
                            id: hoverHandler
                            acceptedDevices: PointerDevice.AllDevices
                        }
                    }

                    Kirigami.PlaceholderMessage {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            top: parent.top
                            // Take into account the header item, which the center anchor doesn't do
                            topMargin: parent.headerItem.height + ((parent.height - parent.headerItem.height) / 2)
                        }
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
