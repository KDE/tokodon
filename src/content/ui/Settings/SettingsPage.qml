// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import QtQuick.Layouts 1.15
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title:window", "Settings")

    leftPadding: 0
    rightPadding: 0

    Connections {
        target: AccountManager

        function onAccountRemoved() {
            if (!AccountManager.hasAccounts) {
                close()
            }
        }
    }

    ColumnLayout {
        GeneralCard {}
        AccountsCard {}
        PreferencesCard {}
        SonnetCard {}

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                Component {
                    id: networkProxyPage
                    NetworkProxyPage {
                    }
                }
                MobileForm.FormButtonDelegate {
                    text: i18n("Network Proxy")
                    onClicked: applicationWindow().pageStack.layers.push(networkProxyPage)
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                Component {
                    id: aboutPage
                    MobileForm.AboutPage {
                        aboutData: About
                    }
                }
                Component {
                    id: aboutKDE
                    MobileForm.AboutKDE {}
                }

                MobileForm.FormButtonDelegate {
                    text: i18n("About Tokodon")
                    onClicked: applicationWindow().pageStack.layers.push(aboutPage)
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormButtonDelegate {
                    text: i18n("About KDE")
                    onClicked: applicationWindow().pageStack.layers.push(aboutKDE)
                }
            }
        }
    }
}
