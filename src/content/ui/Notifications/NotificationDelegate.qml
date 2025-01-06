// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import org.kde.tokodon

Delegates.IndicatorItemDelegate {
    id: root

    default property alias children: inner.children

    required property bool loading

    width: ListView.view.width

    contentItem: Kirigami.FlexColumn {
        id: flexColumn

        maximumWidth: Kirigami.Units.gridUnit * 40
        spacing: 0

        ColumnLayout {
            id: inner
        }
    }
}
