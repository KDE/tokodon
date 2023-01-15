// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0

QQC2.ItemDelegate {
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing * 2
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2
    highlighted: false
    hoverEnabled: false
    width: ListView.view.width
    contentItem: Kirigami.FlexColumn {
        maximumWidth: Kirigami.Units.gridUnit * 40
        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            Kirigami.Icon {
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: Kirigami.Units.iconSizes.small
                source: 'list-add-user'
            }

            Kirigami.Heading {
                level: 4
                text: i18n("%1 followed you", model.notificationActorIdentity.displayNameHtml)
                textFormat: Text.RichText
            }
        }

        RowLayout {
            spacing: Kirigami.Units.largeSpacing * 2
            Kirigami.Avatar {
                Layout.alignment: Qt.AlignTop
                Layout.rowSpan: 5
                source: model.notificationActorIdentity.avatarUrl
                cache: true
                actions.main: Kirigami.Action {
                    onTriggered: pageStack.push("qrc:/content/ui/AccountInfo.qml", {
                        model: model.accountModel,
                    })
                }
                name: model.notificationActorIdentity.displayName
            }
            ColumnLayout {
                Kirigami.Heading {
                    id: heading
                    level: 5
                    text: model.notificationActorIdentity.displayNameHtml
                    type: Kirigami.Heading.Type.Primary
                    color: Kirigami.Theme.textColor
                }
                Kirigami.Heading {
                    level: 5
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    color: Kirigami.Theme.disabledTextColor
                    text: `@${model.notificationActorIdentity.account}`
                }
            }
        }

        Kirigami.Separator {
            visible: root.showSeparator && !model.selected
            Layout.fillWidth: true
        }
    }
    text: i18n("%1 followed you", model.notificationActorIdentity.displayName)
}
