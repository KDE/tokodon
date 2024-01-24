// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.notification 1

import org.kde.tokodon
import org.kde.tokodon.private

MastoPage {
    id: root

    title: i18nc("@title:window", "Notifications")

    FormCard.FormHeader {
        title: i18n("Setup Required")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            labelItem.wrapMode: Text.WordWrap
            text: i18n("Tokodon can show notifications for activity such as users boosting or replying to your posts.\n\nThe types of notifications shown can be fine tuned when logged in.")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: learnMoreButton
            text: i18n("Allow Notifications")
            icon.name: "checkmark-symbolic"
            onClicked: NotificationPermission.requestPermission(success => {
                if (success) {
                    continueButton.clicked();
                }
            })
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: continueButton

            text: i18n("Continue")
            icon.name: "go-next-symbolic"
            onClicked: {
                Config.promptedNotificationPermission = true;
                Config.save();

                // TODO: this should continue onto the next step, but can be expanded later when we have more than two steps.
                applicationWindow().decideDefaultPage();
            }
        }
    }
}
