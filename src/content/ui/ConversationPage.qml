// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    title: i18n("Conversations")
    ListView {
        id: conversationView
        currentIndex: -1

        model: ConversationModel {}

        delegate: Kirigami.AbstractListItem {
            id: root
            property bool showSeparator: model.index !== conversationView.count - 1
            property bool isRead: !model.unread
            leftPadding: Kirigami.Units.gridUnit
            rightPadding: Kirigami.Units.gridUnit
            topPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

            onClicked: {
                const subModel = model.threadModel;
                pageStack.push("qrc:/content/ui/TimelinePage.qml", {
                    model: subModel,
                });
                if (model.unread) {
                    conversationView.model.markAsRead(model.conversationId);
                }
            }

            hoverEnabled: true

            background: Rectangle {
                color: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, root.showSelected ? 0.5 : hoverHandler.hovered ? 0.2 : 0)

                // indicator rectangle
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.topMargin: 1
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 1

                    width: 4
                    visible: !root.isRead
                    color: Kirigami.Theme.highlightColor
                }

                HoverHandler {
                    id: hoverHandler
                    // disable hover input on mobile because touchscreens trigger hover feedback and do not "unhover" in Qt
                    enabled: !Kirigami.Settings.isMobile
                }

                Kirigami.Separator {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: root.leftPadding
                    anchors.rightMargin: root.rightPadding
                    visible: root.showSeparator && !root.showSelected
                    opacity: 0.5
                }
            }

            Item {
                id: item
                implicitHeight: rowLayout.implicitHeight

                RowLayout {
                    id: rowLayout
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Kirigami.Avatar {
                        name: model.authorDisplayName
                        source: model.avatar
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        sourceSize.width: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                        sourceSize.height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                        Layout.preferredWidth: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                        Layout.preferredHeight: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing


                        RowLayout {
                            Layout.fillWidth: true
                            QQC2.Label {
                                Layout.fillWidth: true
                                text: model.authorDisplayName
                                elide: Text.ElideRight
                                font.weight: root.isRead ? Font.Normal : Font.Bold
                            }
                            QQC2.Label {
                                text: model.relativeTime
                                color: Kirigami.Theme.disabledTextColor
                            }
                        }
                        QQC2.Label {
                            color: Kirigami.Theme.disabledTextColor
                            Layout.fillWidth: true
                            maximumLineCount: 1
                            elide: Text.ElideRight
                            text: model.display
                        }
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked: {
                        if (mouse.button === Qt.RightButton) {
                            root.contextMenuRequested();
                        } else if (mouse.button === Qt.LeftButton) {
                            root.clicked();
                        }
                    }
                    onPressAndHold: root.contextMenuRequested();
                }
            }
        }
    }
}