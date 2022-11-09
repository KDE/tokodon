// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import QtQuick.Layouts 1.15

Kirigami.PageRow {
    id: pageStack

    globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar

    initialPage: Kirigami.ScrollablePage {
        title: i18nc("@title:window", "Settings")

        leftPadding: 0
        rightPadding: 0

        ColumnLayout {
            GeneralCard {}
            AccountsCard {}
            SonnetCard {}
        }
    }
}
