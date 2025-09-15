// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kitemmodels

Kirigami.SearchDialog {
    id: root

    model: KSortFilterProxyModel {
        sourceModel: ActionModel {
            id: actionModel
            // TODO: this should just fetch all actions of all collections
            collectionName: "tokodon_actions"
            shownActions: ActionModel.ActiveActions
        }
        sortRole: Qt.DisplayRole
        sortCaseSensitivity: Qt.CaseInsensitive
        filterRole: Qt.DisplayRole
        filterString: root.text
        filterCaseSensitivity: Qt.CaseInsensitive
    }

    delegate: QQC2.ItemDelegate {
        id: commandDelegate

        width: ListView.view.width
        required property int index
        required property QtObject actionDescription
        required property QtObject actionInstance

        icon.name: actionDescription.icon
        text: actionDescription.text

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Icon {
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: implicitWidth
                source: actionDescription.icon.name || actionDescription.icon.source
            }

            QQC2.Label {
                Layout.fillWidth: true
                text: actionDescription.text
            }

            QQC2.Label {
                text: actionDescription?.shortcut ?? ""
                color: Kirigami.Theme.disabledTextColor
            }
        }

        onClicked: {
            actionInstance.trigger()
            root.close()
        }
    }

    emptyText: i18ndc("kirigami-addons6", "@info:placeholder", "No results found")
}
