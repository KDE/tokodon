// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as Components

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    enum Purpose {
        New,
        Edit
    }

    property var purpose

    title: purpose === EditListPage.New ? i18nc("@title:window", "Create List") : i18nc("@title:window", "Edit List")

    property ListEditorBackend backend: ListEditorBackend {
        title: titleField.text
        exclusive: exclusiveField.checked
    }

    data: Connections {
        target: backend

        function onDone() {
            pageStack.layers.pop();
        }
    }

    Component.onCompleted: titleField.forceActiveFocus()

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextFieldDelegate {
            id: titleField
            label: i18nc("@label:textbox List title", "Title")
            onAccepted: exclusiveField.forceActiveFocus()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormCheckDelegate {
            id: exclusiveField
            text: i18nc("@label If the list is exclusive", "Exclusive")
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormButtonDelegate {
            id: createButton
            text: i18nc("@action:button Create the list", "Create")
            onClicked: backend.create()
        }
    }
}
