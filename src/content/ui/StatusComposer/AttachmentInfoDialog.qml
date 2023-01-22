// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

QQC2.Dialog {
    id: root

    property alias text: textArea.text

    title: i18n("Add a description")

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    implicitHeight: Kirigami.Units.gridUnit * 20
    implicitWidth: Kirigami.Units.gridUnit * 20

    onApplied: root.close()
    onRejected: root.close()

    contentItem: ColumnLayout {
        QQC2.TextArea {
            id: textArea
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    standardButtons: QQC2.Dialog.Apply | QQC2.Dialog.Cancel
}
