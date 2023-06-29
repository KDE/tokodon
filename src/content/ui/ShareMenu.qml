// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-Licence-Identifier: LGPL-2.0-or-later

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami
import org.kde.purpose 1.0 as Purpose

QQC2.Menu {
    id: root

    required property string url

    title: i18n("Share")
    Repeater {
        model: Purpose.PurposeAlternativesModel {
            id: alternativesModel
            inputData: {
                'urls': [root.url],
                'title': "Post",
                'mimeType': "*"
            }
            pluginType: "ShareUrl"
        }

        delegate: QQC2.MenuItem {
            text: model.display
            icon.name: model.iconName

            onTriggered: {
                applicationWindow().pageStack.pushDialogLayer('qrc:/content/ui/ShareDialog.qml', {
                    index: index,
                    model: alternativesModel
                })
            }
        }
    }
}