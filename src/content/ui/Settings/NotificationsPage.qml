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
            description: {
                if (Controller.pushNotificationsAvailable) {
                    if (root.config.enablePushNotifications) {
                        return i18n("Notifications can appear even when Tokodon isn't running.");
                    } else {
                        return i18n("Push notifications are available but could not be enabled. Please log out and log back in.");
                    }
                } else {
                    return i18n("Notifications will only appear when Tokodon is running.");
                }
            }
            checked: root.config.enableNotifications
            onToggled: {
                root.config.enableNotifications = checked;
                root.saveConfig();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Filtering Policy")
    }

    component PolicyCombo: FormCard.FormComboBoxDelegate {
        Layout.fillWidth: true

        textRole: "display"
        valueRole: "value"
        model: [
            {
                display: i18nc("@info:Option Accept and show this in notifications", "Accept"),
                value: "accept"
            },
            {
                display: i18nc("@info:Option Filter and send to the filtered inbox", "Filter"),
                value: "filter"
            },
            {
                display: i18nc("@info:Option Ignore completely, do not show notification", "Ignore"),
                value: "drop"
            },
        ]
    }

    FormCard.FormCard {
        PolicyCombo {
            text: i18nc("@label Notification preferences", "People you don't follow")
            description: i18nc("@label Notification preferences", "Until you manually approve them.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowing)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowing = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: i18nc("@label Notification preferences", "People not following you")
            description: i18nc("@label Notification preferences", "Including people who have been following you fewer than 3 days.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowers)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowers = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: i18nc("@label Notification preferences", "New accounts")
            description: i18nc("@label Notification preferences", "Created within the past 30 days.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forNewAccounts)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forNewAccounts = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: i18nc("@label Notification preferences", "Unsolicited conversations")
            description: i18nc("@label Notification preferences", "Filtered unless it's in reply to your own mention or if you follow the sender.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forPrivateMentions)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forPrivateMentions = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: i18nc("@label Notification preferences", "Moderated accounts")
            description: i18nc("@label Notification preferences", "Limited by server moderators.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forLimitedAccounts)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forLimitedAccounts = currentValue
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Events")
    }

    FormCard.FormCard {
        enabled: root.config.enableNotifications

        FormCard.FormSwitchDelegate {
            id: mentionsDelegate
            text: i18n("Mentions")
            description: i18n("When someone mentions you in a new post, or replies to one of your threads.")
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
            description: i18n("When a user you have notifications turned on for makes a new post.")
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
            description: i18n("When someone boosted one of your posts.")
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
            description: i18n("When someone follows you.")
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
            description: i18n("When an account who requires manual approval wants to follow you.")
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
            description: i18n("When a post you made was favorited by another user.")
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
            description: i18n("When a poll you voted in has ended.")
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
            description: i18n("When a post you interacted with was edited by the author.")
            checked: root.config.notifyUpdate
            onToggled: {
                root.config.notifyUpdate = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: adminSignUpDelegate
            text: i18n("Server Sign-ups")
            description: i18n("When someone signs up to your server.")
            checked: root.config.notifySignup
            visible: root.account.identity.permission & AdminAccountInfo.ManageUsers
            onToggled: {
                root.config.notifySignup = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {
            visible: adminSignUpDelegate.visible
        }

        FormCard.FormSwitchDelegate {
            id: adminReportDelegate
            text: i18n("Server Reports")
            description: i18n("When someone files a report against a user on your server.")
            checked: root.config.notifyReport
            visible: root.account.identity.permission & AdminAccountInfo.ManageUsers
            onToggled: {
                root.config.notifyReport = checked;
                root.saveConfig();
            }
        }

        FormCard.FormDelegateSeparator {
            visible: adminReportDelegate.visible
        }

        FormCard.FormSwitchDelegate {
            id: relationshipsDelegate
            text: i18n("Severed Relationships")
            description: i18n("When you or your server moderates another server, which you are following users or have followers with.")
            checked: root.config.notifyRelationships
            onToggled: {
                root.config.notifyRelationships = checked;
                root.saveConfig();
            }
        }
    }
}
