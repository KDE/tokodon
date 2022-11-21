// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import QtQuick.Layouts 1.15
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kmasto 1.0

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

            MobileForm.FormCard {
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                contentItem: ColumnLayout {
                    spacing: 0
                    Component {
                        id: aboutPage
                        MobileForm.AboutPage {
                            aboutData: About.aboutData
                        }
                    }
                    MobileForm.FormButtonDelegate {
                        text: i18n("About Tokodon")
                        onClicked: applicationWindow().pageStack.layers.push(aboutPage)
                    }
                }
            }
        }
    }
}
