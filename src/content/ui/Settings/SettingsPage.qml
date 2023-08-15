// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.18 as Kirigami
import QtQuick.Layouts 1.15
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.kmasto 1.0

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Settings")

    data: Connections {
        target: AccountManager

        function onAccountRemoved() {
            if (!AccountManager.hasAccounts) {
                close()
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("General")
    }

    GeneralCard {}

    FormCard.FormHeader {
        title: i18n("Accounts")
    }

    AccountsCard {}

    FormCard.FormHeader {
        title: i18nc("@label Settings header", "Preferences")
    }

    PreferencesCard {}

    FormCard.FormHeader {
        visible: Qt.platform.os !== "android"
        title: i18n("Spellchecking")
    }

    Loader {
        Layout.fillWidth: true

        active: Qt.platform.os !== "android"
        source: "qrc:/content/ui/Settings/SonnetCard.qml"
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        data: Component {
            id: networkProxyPage
            NetworkProxyPage {}
        }

        FormCard.FormButtonDelegate {
            text: i18n("Network Proxy")
            onClicked: applicationWindow().pageStack.layers.push(networkProxyPage)
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        data: [
            Component {
                id: aboutPage
                FormCard.AboutPage {
                    aboutData: About
                }
            },
            Component {
                id: aboutKDE
                FormCard.AboutKDE {}
            }
        ]

        FormCard.FormButtonDelegate {
            id: aboutTokodon
            text: i18n("About Tokodon")
            onClicked: applicationWindow().pageStack.layers.push(aboutPage)
        }

        FormCard.FormDelegateSeparator { above: aboutKde; below: aboutTokodon }

        FormCard.FormButtonDelegate {
            id: aboutKde
            text: i18n("About KDE")
            onClicked: applicationWindow().pageStack.layers.push(aboutKDE)
        }
    }
}
