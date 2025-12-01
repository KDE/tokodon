// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

MastoPage {
    id: root

    title: i18nc("@title:window", "Welcome")

    Image {
        source: "qrc:/org.kde.tokodon.svg"

        fillMode: Image.PreserveAspectFit

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: Kirigami.Units.largeSpacing * 3
    }

    FormCard.FormHeader {
        title: i18n("Welcome to Tokodon")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: i18n("Some initial setup is required before you can use Tokodon.")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: learnMoreButton
            text: i18n("Continue")
            icon.name: "go-next-symbolic"
            onClicked: applicationWindow().pageStack.push(Qt.createComponent("org.kde.tokodon", InitialSetupFlow.getNextStep()));
        }
    }
}
