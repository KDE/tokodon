// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigami.private.polyfill // remove once we depend on Qt 6.9
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon

import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.OverlayDrawer {
    id: drawer

    required property TokodonApplication application
    required property bool shouldCollapse

    property alias actions: actionsRepeater.model
    property alias bottomActions: bottomActionsRepeater.model

    edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    modal: shouldCollapse || !enabled
    z: modal ? Math.round(position * 10000000) : 100
    width: Kirigami.Units.gridUnit * 14
    Behavior on width {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }
    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    handleVisible: modal && !isShowingFullScreenImage && enabled
    onModalChanged: drawerOpen = !modal

    leftPadding: parent.SafeArea.margins.left
    rightPadding: parent.SafeArea.margins.right
    topPadding: parent.SafeArea.margins.top
    bottomPadding: parent.SafeArea.margins.bottom

    /// Handles unchecking extra actions that are not part of the main group (e.g. favorited lists)
    function uncheckAuxiliaryActions(): void {
        // favorited lists
        for (let i = 0; i < favoriteListRepeater.count; i++) {
            const actionDelegate = favoriteListRepeater.itemAt(i);
            actionDelegate.checked = false;
        }
    }

    component ActionDelegate: Delegates.RoundedItemDelegate {
        id: delegate

        property int alertCount

        padding: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        activeFocusOnTab: true

        onClicked: {
            if (delegate.checkable) {
                drawer.uncheckAuxiliaryActions();
                checked = true;
            }
            if (drawer.modal) {
                drawer.close();
            }
        }

        // Notification indicator
        Rectangle {
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: Kirigami.Units.largeSpacing
            }

            color: Kirigami.Theme.highlightColor

            width: 20
            height: width
            radius: width
            visible: delegate.alertCount > 0

            QQC2.Label {
                anchors {
                    centerIn: parent
                }

                text: delegate.alertCount
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        QQC2.ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: pageStack.globalToolBar.preferredHeight
            Layout.bottomMargin: Kirigami.Units.smallSpacing / 2

            visible: !drawer.shouldCollapse

            contentItem: SearchField {}
        }

        UserInfo {
            Layout.fillWidth: true
            application: drawer.application
            sidebar: drawer
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.margins: scrollView.QQC2.ScrollBar.vertical.visible ? 0 : Kirigami.Units.smallSpacing
            Layout.topMargin: Math.round(Kirigami.Units.smallSpacing / 2)
            Layout.bottomMargin: 0
        }

        QQC2.ScrollView {
            id: scrollView

            contentWidth: availableWidth
            topPadding: Math.round(Kirigami.Units.smallSpacing / 2)
            bottomPadding: Math.round(Kirigami.Units.smallSpacing / 2)

            QQC2.ScrollBar.vertical.interactive: false

            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 0

                width: scrollView.contentWidth
                height: Math.max(scrollView.availableHeight, implicitHeight)

                Repeater {
                    id: actionsRepeater

                    delegate: ActionDelegate {
                        required property var modelData

                        action: modelData
                        visible: modelData.visible
                        alertCount: modelData.alertCount ?? 0
                    }
                }

                Item {
                    Layout.preferredHeight: Kirigami.Units.gridUnit
                }

                Repeater {
                    id: favoriteListRepeater

                    model: FavoriteListsModel {
                        account: AccountManager.selectedAccount
                    }

                    delegate: ActionDelegate {
                        id: delegate

                        required property string id
                        required property string name

                        action: Kirigami.Action {
                            icon.name: "view-list-text"
                            text: delegate.name
                            checkable: true

                            onTriggered: {
                                pageStack.clear();
                                Navigation.openList(delegate.id, delegate.name);
                                checked = true;
                                drawer.application.uncheckMainActions();
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                Repeater {
                    id: bottomActionsRepeater

                    delegate: ActionDelegate {
                        required property var modelData

                        action: modelData
                        visible: modelData.visible
                        alertCount: modelData.alertCount ?? 0
                    }
                }
            }
        }
    }
}
