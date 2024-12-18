// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.tokodon
import "PostDelegate"

Kirigami.ScrollablePage {
    id: root

    title: i18n("Advanced View")

    horizontalScrollBarPolicy: QQC2.ScrollBar.AsNeeded

    function openPost(id: string) : void {
        console.info(id);
    }

    RowLayout {
        spacing: Kirigami.Units.largeSpacing * 2
        implicitHeight: root.availableHeight - root.topPadding - root.bottomPadding

        component PageColumn: Rectangle {
            property alias title: heading.text
            default property alias children: layout.children

            Layout.preferredWidth: 400
            Layout.fillHeight: true

            radius: Kirigami.Units.cornerRadius

            border {
                width: 1
                color: Qt.alpha(Kirigami.Theme.disabledTextColor, 0.2)
            }

            color: Kirigami.Theme.backgroundColor

            ColumnLayout {
                id: layout

                spacing: 0

                anchors.fill: parent

                Kirigami.Heading {
                    id: heading

                    verticalAlignment: Qt.AlignVCenter

                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.mediumSpacing
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }
            }
        }

        PageColumn {
            title: i18n("Home")

            Kirigami.Theme.colorSet: Kirigami.Theme.View

            QQC2.ScrollView {
                clip: true

                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    model: MainTimelineModel {
                        name: "home"
                    }
                    delegate: PostDelegate {
                        id: status

                        timelineModel: root.model
                        expandedPost: false
                        showSeparator: index !== ListView.view.count - 1
                        loading: root.model.loading
                        width: ListView.view.width
                    }
                }
            }
        }

        PageColumn {
            title: i18n("Notifications")

            Kirigami.Theme.colorSet: Kirigami.Theme.View

            QQC2.ScrollView {
                clip: true

                Layout.fillWidth: true
                Layout.fillHeight: true

                NotificationsView {
                }
            }
        }

        PageColumn {
            title: i18n("Local")

            Kirigami.Theme.colorSet: Kirigami.Theme.View

            QQC2.ScrollView {
                clip: true

                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    model: MainTimelineModel {
                        name: "public"
                    }
                    delegate: PostDelegate {
                        id: status

                        timelineModel: root.model
                        expandedPost: false
                        showSeparator: index !== ListView.view.count - 1
                        loading: root.model.loading
                        width: ListView.view.width
                    }
                }
            }
        }

        PageColumn {
            id: extraColumn

            title: i18n("Empty")
        }

        Item {
            id: spacer
            Layout.fillWidth: true
        }
    }
}
