// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

QQC2.Dialog {
    id: root

    property alias text: textArea.text

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    implicitHeight: Kirigami.Units.gridUnit * 20
    implicitWidth: Kirigami.Units.gridUnit * 20

    padding: Kirigami.Units.largeSpacing
    bottomPadding: 0

    modal: true

    standardButtons: QQC2.Dialog.Apply | QQC2.Dialog.Cancel

    onApplied: root.close()
    onRejected: root.close()

    contentItem: ColumnLayout {
        Kirigami.Heading {
            text: i18n("Add a description")
            Layout.fillWidth: true
        }

        QQC2.TextArea {
            id: textArea
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Component.onCompleted: footer.padding = Kirigami.Units.largeSpacing;

    background: Kirigami.ShadowedRectangle {
        radius: 7
        color: Kirigami.Theme.backgroundColor

        border {
            width: 1
            color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.3);
        }

        shadow {
            size: Kirigami.Units.gridUnit
            yOffset: 4
            color: Qt.rgba(0, 0, 0, 0.2)
        }

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }
}
