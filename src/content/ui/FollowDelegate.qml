// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0

QQC2.ItemDelegate {
    topPadding: Kirigami.Units.largeSpacing * 2
    bottomPadding: Kirigami.Units.largeSpacing * 2
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2
    highlighted: false
    hoverEnabled: false
    width: ListView.view.width
    background: Item {
        Kirigami.Separator {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
            }
        }
    }
    bottomInset: 2
    contentItem: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        RowLayout {
            Kirigami.Icon {
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: Kirigami.Units.iconSizes.small
                source: 'list-add-user'
            }

            Kirigami.Heading {
                level: 4
                text: i18n("%1 followed you", model.newFollowerIdentity.displayNameHtml)
                textFormat: Text.RichText
            }
        }

        RowLayout {
            spacing: Kirigami.Units.largeSpacing * 2
            Kirigami.Avatar {
                Layout.alignment: Qt.AlignTop
                Layout.rowSpan: 5
                source: model.newFollowerIdentity.avatarUrl
                cache: true
                actions.main: Kirigami.Action {
                    onTriggered: pageStack.push("qrc:/content/ui/AccountInfo.qml", {
                        model: model.accountModel,
                    })
                }
                name: model.newFollowerIdentity.displayName
            }
            ColumnLayout {
                Kirigami.Heading {
                    id: heading
                    level: 5
                    text: model.newFollowerIdentity.displayNameHtml
                    type: Kirigami.Heading.Type.Primary
                    color: Kirigami.Theme.textColor
                }
                Kirigami.Heading {
                    level: 5
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    color: Kirigami.Theme.disabledTextColor
                    text: `@${model.newFollowerIdentity.account}`
                }
            }
        }
    }
    text: i18n("%1 followed you", model.newFollowerIdentity.displayName)
}
