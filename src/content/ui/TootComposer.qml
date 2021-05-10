// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

MastoPage {
    title: i18n("Write a new toot")

    Kirigami.FlexColumn {
        padding: 0
        QQC2.TextField {
            placeholderText: i18n("Content Warning")
            Layout.fillWidth: true
        }
        QQC2.TextArea {
            placeholderText: i18n("What's new?")
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 15
        }
        RowLayout {
            QQC2.ToolButton {
                icon.name: "mail-attachment-symbolic"
            }
            QQC2.ToolButton {
                icon.name: "gnumeric-graphguru"
            }
            QQC2.ToolButton {
                icon.name: "kstars_xplanet"
                onClicked: visibilityMenu.open()
                QQC2.Menu {
                    id: visibilityMenu
                    QQC2.MenuItem {
                        icon.name: "kstars_xplanet"
                        text: i18n("Public")
                    }
                    QQC2.MenuItem {
                        icon.name: "unlock"
                        text: i18n("Unlisted")
                    }
                    QQC2.MenuItem {
                        text: i18n("Private")
                        icon.name: "lock"
                    }
                    QQC2.MenuItem {
                        text: i18n("Direct Message")
                        icon.name: "mail-message"
                    }
                }
            }
            QQC2.ToolButton {
                text: i18nc("Short for content warning", "cw")
            }
        }
    }
}
