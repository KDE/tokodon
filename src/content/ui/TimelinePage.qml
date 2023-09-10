// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kirigamiaddons.labs.components 1.0 as LabComponents
import org.kde.kirigamiaddons.components 1.0 as Components
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import './StatusDelegate'
import './StatusComposer'

Kirigami.ScrollablePage {
    id: root

    property var dialog: null
    required property var model
    property bool expandedPost: false
    property alias listViewHeader: listview.header
    property alias showPostAction: postAction.visible

    title: model.displayName
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight

        text: root.title

        textFormat: TextEdit.RichText
    }

    header: LabComponents.Banner
    {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width

        showCloseButton: true

        actions: Kirigami.Action {
            text: i18n("Settings")
            icon.name: "settings-configure"
            onTriggered: pageStack.pushDialogLayer('qrc:/content/ui/Settings/SettingsPage.qml', {}, { title: i18n("Configure") })
        }
    }

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    actions.main: Kirigami.Action {
        id: postAction
        icon.name: "list-add"
        text: i18n("Post")
        enabled: AccountManager.hasAccounts
        onTriggered: Navigation.openStatusComposer()
    }

    Connections {
        target: Navigation
        function onOpenFullScreenImage(attachments, identity, currentIndex) {
            if (root.isCurrentPage) {
                root.dialog = fullScreenImage.createObject(parent, {
                    attachments: attachments,
                    identity: identity,
                    initialIndex: currentIndex,
                });
                root.dialog.open();
            }
        }
    }

    Connections {
        target: Controller
        function onNetworkErrorOccurred(error) {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your settings.", error)
            message.visible = true
        }
    }

    Connections {
        target: root.model
        function onPostSourceReady(backend, isEdit) {
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: isEdit ? StatusComposer.Edit : StatusComposer.Redraft,
                backend: backend
            });
        }
    }

    ListView {
        id: listview
        model: root.model

        // HACK: Temporary workaround to help with performance issues
        cacheBuffer: 100
        Component {
            id: fullScreenImage
            FullScreenImage {}
        }

        delegate: StatusDelegate {
            id: status

            timelineModel: root.model
            expandedPost: root.expandedPost
            showSeparator: index !== ListView.view.count - 1
            loading: listview.model.loading
            Layout.fillWidth: true

            Connections {
                target: listview

                function onContentYChanged() {
                    const aMin = status.y
                    const aMax = status.y + status.height

                    const bMin = listview.contentY
                    const bMax = listview.contentY + listview.height

                    if (!root.isCurrentPage) {
                        status.inViewPort = false
                        return
                    }

                    let topEdgeVisible
                    let bottomEdgeVisible

                    // we are still checking two rectangles, but if one is bigger than the other
                    // just switch which one should be checked.
                    if (status.height > listview.height) {
                        topEdgeVisible = bMin > aMin && bMin < aMax
                        bottomEdgeVisible = bMax > aMin && bMax < aMax
                    } else {
                        topEdgeVisible = aMin > bMin && aMin < bMax
                        bottomEdgeVisible = aMax > bMin && aMax < bMax
                    }

                    status.inViewPort = topEdgeVisible || bottomEdgeVisible
                }
            }

            Connections {
                target: root

                function onIsCurrentPageChanged() {
                    if (!root.isCurrentPage) {
                        status.inViewPort = false
                    } else {
                        listview.onContentYChanged()
                    }
                }
            }

            Connections {
                target: appwindow

                function onIsShowingFullScreenImageChanged() {
                    if (appwindow.isShowingFullScreenImage) {
                        status.inViewPort = false
                    } else {
                        listview.onContentYChanged()
                    }
                }
            }
        }

        Rectangle {
            anchors.fill: parent
            anchors {
                topMargin: listview.headerItem ? listview.headerItem.height : 0
            }

            visible: listview.model.loading

            color: Kirigami.Theme.backgroundColor

            QQC2.ProgressBar {
                anchors.centerIn: parent
                indeterminate: true
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No posts")
            visible: !listview.model.loading && listview.count === 0
            width: parent.width - Kirigami.Units.gridUnit * 4
        }

        Components.FloatingButton {
            opacity: listview.atYBeginning ? 0 : 1
            visible: opacity !== 0

            Behavior on opacity {
                NumberAnimation {
                }
            }

            anchors {
                right: parent.right
                rightMargin: Kirigami.Units.largeSpacing
                bottom: parent.bottom
                bottomMargin: Kirigami.Units.largeSpacing
            }

            action: Kirigami.Action
            {
                icon.name: "arrow-up"
                onTriggered: listview.positionViewAtBeginning()
            }
        }
    }
}
