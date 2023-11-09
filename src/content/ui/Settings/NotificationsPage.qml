// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml
import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.tokodon
import org.kde.tokodon.private

FormCard.FormCardPage {
    id: root

    property var account: AccountManager.selectedAccount
    readonly property var config: account.config

    function saveConfig() {
        config.save();
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormSwitchDelegate {
            text: i18n("Enable notifications for this account")
            description: root.config.enablePushNotifications ? i18n("Push notifications are enabled.") : i18n("Push notifications are not available for this account. Please log out and log back in.")
            checked: root.config.enableNotifications
            onToggled: {
                root.config.enableNotifications = checked;
                root.saveConfig();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Types")
    }

    FormCard.FormCard {
        enabled: root.config.enableNotifications

        FormCard.FormSwitchDelegate {
            id: mentionsDelegate
            text: i18n("Mentions")
            checked: root.config.notifyMention
            onToggled: {
                root.config.notifyMention = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: statusesDelegate
            text: i18n("Statuses")
            checked: root.config.notifyStatus
            onToggled: {
                root.config.notifyStatus = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: boostsDelegate
            text: i18n("Boosts")
            checked: root.config.notifyBoost
            onToggled: {
                root.config.notifyBoost = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: followersDelegate
            text: i18n("New followers")
            checked: root.config.notifyFollow
            onToggled: {
                root.config.notifyFollow = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: requestsDelegate
            text: i18n("New follow requests")
            checked: root.config.notifyFollowRequest
            onToggled: {
                root.config.notifyFollowRequest = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: favoritesDelegate
            text: i18n("Favorites")
            checked: root.config.notifyFavorite
            onToggled: {
                root.config.notifyFavorite = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: pollsDelegate
            text: i18n("Polls")
            checked: root.config.notifyPoll
            onToggled: {
                root.config.notifyPoll = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: editsDelegate
            text: i18n("Edits")
            checked: root.config.notifyUpdate
            onToggled: {
                root.config.notifyUpdate = checked;
                root.saveConfig();
            }
        }
    }
}