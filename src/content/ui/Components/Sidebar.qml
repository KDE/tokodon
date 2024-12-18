// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon
import org.kde.tokodon.private
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

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    component ActionDelegate: Delegates.RoundedItemDelegate {
        id: delegate

        property int alertCount

        QQC2.ButtonGroup.group: pageButtonGroup

        padding: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        activeFocusOnTab: true

        onClicked: {
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

            leftPadding: 3
            rightPadding: 3
            topPadding: 3
            bottomPadding: 3

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

            QQC2.ScrollBar.vertical.interactive: false

            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 0

                width: scrollView.contentWidth
                height: Math.max(scrollView.availableHeight, implicitHeight)

                QQC2.ButtonGroup {
                    id: pageButtonGroup
                }

                Repeater {
                    id: actionsRepeater

                    delegate: ActionDelegate {
                        required property var modelData

                        action: modelData
                        visible: modelData.visible
                        enabled: !AccountManager.selectedAccountHasIssue
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