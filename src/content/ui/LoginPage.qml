// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kmasto 1.0

MastoPage {
    title: i18n("Login")
    Kirigami.FormLayout {
        anchors.centerIn: parent
        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Welcome to Tokodon")
        }
        QQC2.TextField {
            id: instanceUrl
            Kirigami.FormData.label: i18n("Instance Url:")
        }
        QQC2.TextField {
            id: username
            Kirigami.FormData.label: i18n("Username:")
        }
        QQC2.Button {
            text: i18n("Continue")
            onClicked: pageStack.layers.push('qrc:/content/ui/AuthorizationPage.qml', {
                account: AccountManager.createNewAccount(username.text, instanceUrl.text)
            });
        }
    }
}
