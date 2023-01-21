// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import "./StatusDelegate"

QQC2.ItemDelegate {
    id: root

    required property int index
    required property var notificationActorIdentity
    required property bool selected

    topPadding: 0
    bottomPadding: Kirigami.Units.smallSpacing
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2

    highlighted: false
    hoverEnabled: false

    width: ListView.view.width

    background: null

    contentItem: Kirigami.FlexColumn {
        maximumWidth: Kirigami.Units.gridUnit * 40
        spacing: 0

        RowLayout {
            Kirigami.Icon {
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: Kirigami.Units.iconSizes.small
                source: 'list-add-user'
            }

            Kirigami.Heading {
                level: 4
                text: i18n("%1 followed you", root.notificationActorIdentity.displayNameHtml)
                textFormat: Text.RichText
            }
        }

        InlineIdentityInfo {
            secondary: false
            identity: root.notificationActorIdentity
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }
    text: i18n("%1 followed you", root.notificationActorIdentity.displayName)
}
