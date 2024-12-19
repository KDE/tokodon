// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-only

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates

import org.kde.tokodon

import "./PostDelegate"

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title Server-wide announcements set by admins.", "Announcements")

    ListView {
        id: listview

        model: AnnouncementModel {
            id: model
        }
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property var index
            required property string id
            required property string content
            required property date publishedAt
            required property var reactions

            contentItem: ColumnLayout {
                id: layout

                spacing: 0
                clip: true

                Kirigami.Heading {
                    text: i18nc("@label An announcement was published on a date", "Announcement on %1", delegate.publishedAt.toLocaleDateString())
                    type: Kirigami.Heading.Type.Primary
                    level: 4
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                QQC2.Label {
                    Layout.fillWidth: true

                    text: delegate.content
                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText

                    onLinkActivated: (link) => Qt.openUrlExternally(link)
                }

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Repeater {
                        model: delegate.reactions

                        QQC2.Button {
                            id: reactionDelegate

                            required property string name
                            required property string url
                            required property int count
                            required property bool me

                            readonly property bool isCustom: url !== ""

                            padding: Kirigami.Units.smallSpacing

                            checkable: true
                            checked: me

                            onToggled: {
                                if (checked) {
                                    model.addReaction(model.index(delegate.index, 0), reactionDelegate.name);
                                } else {
                                    model.removeReaction(model.index(delegate.index, 0), reactionDelegate.name);
                                }
                            }

                            contentItem: RowLayout {
                                spacing: Kirigami.Units.smallSpacing

                                Item {
                                    Layout.preferredWidth: Kirigami.Units.iconSizes.sizeForLabels

                                    QQC2.Label {
                                        anchors.centerIn: parent

                                        text: reactionDelegate.name
                                        visible: !reactionDelegate.isCustom
                                    }

                                    Image {
                                        anchors.centerIn: parent

                                        source: reactionDelegate.url
                                        visible: reactionDelegate.isCustom

                                        sourceSize.width: Kirigami.Units.iconSizes.sizeForLabels
                                        sourceSize.height: Kirigami.Units.iconSizes.sizeForLabels
                                    }
                                }

                                QQC2.Label {
                                    id: countLabel

                                    text: reactionDelegate.count
                                }
                            }
                        }
                    }
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "note"
            text: i18nc("@label", "No Announcements")
            explanation: i18n("Your server has not made any announcements yet.")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
