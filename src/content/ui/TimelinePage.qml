// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    id: timelinePage
    title: model.displayName

    required property var model

    ListView {
        model: timelinePage.model
        delegate: Kirigami.BasicListItem {
            topPadding: Kirigami.Units.largeSpacing
            leftPadding: Kirigami.Units.smallSpacing
            rightPadding: Kirigami.Units.smallSpacing
            highlighted: false
            hoverEnabled: false
            onClicked: pageStack.push("qrc:/content/ui/TimelinePage.qml", {
                model: model.threadModel
            })
            GridLayout {
                columnSpacing: Kirigami.Units.largeSpacing
                columns: 2

                Kirigami.Icon {
                    source: "retweet"
                    Layout.alignment: Qt.AlignRight
                    visible: model.wasReblogged
                    color: Kirigami.Theme.disabledTextColor
                    Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                    Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
                }
                QQC2.Label {
                    visible: model.wasReblogged
                    text: i18n("Shared by %1", model.rebloggedDisplayName)
                    color: Kirigami.Theme.disabledTextColor
                    font: Kirigami.Theme.smallFont
                }

                Kirigami.Avatar {
                    Layout.alignment: Qt.AlignTop
                    Layout.rowSpan: 4
                    source: model.avatar
                    name: model.authorDisplayName
                }
                RowLayout {
                    Layout.fillWidth: true
                    QQC2.Label {
                        text: model.authorDisplayName
                    }
                    QQC2.Label {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        color: Kirigami.Theme.disabledTextColor
                        text: `@${model.authorId}`
                    }
                    QQC2.Label {
                        text: model.relativeTime
                    }
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    text: model.display
                    wrapMode: Text.Wrap
                }

                GridLayout {
                    id: attachmentGrid
                    Layout.fillWidth: true
                    columns: model.attachments.length > 1 ? 2 : 1
                    Repeater {
                        model: attachments
                        Image {
                            Layout.fillWidth: true
                            Layout.maximumWidth: sourceSize.width
                            Layout.maximumHeight: width / sourceSize.width * sourceSize.height 
                            source: modelData.previewUrl
                        }
                    }
                }

                RowLayout {
                    QQC2.ToolButton {
                        icon.name: "mail-replied-symbolic"
                        text: model.repliesCount < 2 ? model.repliesCount : "1+"
                    }
                    QQC2.ToolButton {
                        icon.name: "retweet"
                        icon.color: model.reblogged ? Kirigami.Theme.negativeTextColor : Kirigami.Theme.textColor
                        onClicked: timelineModel.actionRepeat(timelineModel.index(model.index, 0))
                    }
                    QQC2.ToolButton {
                        icon.name: "emblem-favorite-symbolic"
                        icon.color: model.favorite ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.textColor
                        onClicked: timelineModel.actionFavorite(timelineModel.index(model.index, 0))
                    }
                }
            }
        }
    }
}
