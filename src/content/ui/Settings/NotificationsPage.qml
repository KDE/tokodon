// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    property var account: AccountManager.selectedAccount
    readonly property var config: account.config

    function saveConfig(): void {
        config.save();
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

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
                root.account.updatePushNotifications();
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

    component EventControl: FormCard.AbstractFormDelegate {
        id: eventDelegate

        required property string description
        required property bool shouldDisplay // Called this to not interfere with the AbstractButton property
        required property bool notify

        signal toggledDisplay(display: bool)
        signal toggledNotify(notify: bool)

        contentItem: RowLayout {
            spacing: Kirigami.Units.largeSpacing

            ColumnLayout {
                spacing: 0

                QQC2.Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    text: eventDelegate.text
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    text: eventDelegate.description
                    wrapMode: Text.WordWrap
                    color: Kirigami.Theme.disabledTextColor
                }
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight

                QQC2.Button {
                    text: checked ? i18nc("@action:button", "Hide in All") : i18nc("@action:button", "Show in All")
                    icon.name: checked ? "view-visible-symbolic" : "view-visible-off-symbolic"
                    display: QQC2.AbstractButton.IconOnly

                    checkable: true
                    checked: eventDelegate.shouldDisplay
                    down: checked
                    onToggled: {
                        eventDelegate.toggledDisplay(checked);
                        root.saveConfig();
                    }

                    Accessible.name: text

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
                QQC2.Button {
                    text: checked ? i18nc("@action:button", "Disable notifications") : i18nc("@action:button", "Enable notifications")
                    icon.name: checked ? "notifications" : "notifications-disabled"
                    display: QQC2.AbstractButton.IconOnly

                    checkable: true
                    checked: eventDelegate.notify
                    down: checked
                    onToggled: {
                        eventDelegate.toggledNotify(checked);
                        root.saveConfig();
                    }

                    Accessible.name: text

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
            }
        }
    }

    FormCard.FormCard {
        enabled: root.config.enableNotifications

        EventControl {
            id: mentionsDelegate

            text: i18n("Mentions")
            description: i18n("When someone mentions you in a new post, or replies to one of your threads.")

            shouldDisplay: root.config.displayMention
            onToggledDisplay: display => root.config.displayMention = display

            notify: root.config.notifyMention
            onToggledNotify: notify => root.config.notifyMention = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: quotesDelegate

            text: i18n("Quotes")
            description: i18n("When someone quotes you in a new post")

            shouldDisplay: root.config.displayQuote
            onToggledDisplay: display => root.config.displayQuote = display

            notify: root.config.notifyQuote
            onToggledNotify: notify => root.config.notifyQuote = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: statusesDelegate

            text: i18n("Statuses")
            description: i18n("When a user you have notifications turned on for makes a new post.")

            shouldDisplay: root.config.displayStatus
            onToggledDisplay: display => root.config.displayStatus = display

            notify: root.config.notifyStatus
            onToggledNotify: notify => root.config.notifyStatus = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: boostsDelegate

            text: i18n("Boosts")
            description: i18n("When someone boosted one of your posts.")

            shouldDisplay: root.config.displayBoost
            onToggledDisplay: display => root.config.displayBoost = display

            notify: root.config.notifyBoost
            onToggledNotify: notify => root.config.notifyBoost = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: followersDelegate

            text: i18n("New followers")
            description: i18n("When someone follows you.")

            shouldDisplay: root.config.displayFollow
            onToggledDisplay: display => root.config.displayFollow = display

            notify: root.config.notifyFollow
            onToggledNotify: notify => root.config.notifyFollow = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: requestsDelegate

            text: i18n("New follow requests")
            description: i18n("When an account who requires manual approval wants to follow you.")

            shouldDisplay: root.config.displayFollowRequest
            onToggledDisplay: display => root.config.displayFollowRequest = display

            notify: root.config.notifyFollowRequest
            onToggledNotify: notify => root.config.notifyFollowRequest = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: favoritesDelegate

            text: i18n("Favorites")
            description: i18n("When a post you made was favorited by another user.")

            shouldDisplay: root.config.displayFavorite
            onToggledDisplay: display => root.config.displayFavorite = display

            notify: root.config.notifyFavorite
            onToggledNotify: notify => root.config.notifyFavorite = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: pollsDelegate

            text: i18n("Polls")
            description: i18n("When a poll you voted in has ended.")

            shouldDisplay: root.config.displayPoll
            onToggledDisplay: display => root.config.displayPoll = display

            notify: root.config.notifyPoll
            onToggledNotify: notify => root.config.notifyPoll = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: editsDelegate

            text: i18n("Edits")
            description: i18n("When a post you interacted with was edited by the author.")

            shouldDisplay: root.config.displayUpdate
            onToggledDisplay: display => root.config.displayUpdate = display

            notify: root.config.notifyUpdate
            onToggledNotify: notify => root.config.notifyUpdate = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: adminSignUpDelegate

            text: i18n("Server Sign-ups")
            description: i18n("When someone signs up to your server.")
            visible: root.account.identity.permission & AdminAccountInfo.ManageUsers

            shouldDisplay: root.config.displaySignup
            onToggledDisplay: display => root.config.displaySignup = display

            notify: root.config.notifySignup
            onToggledNotify: notify => root.config.notifySignup = notify
        }

        FormCard.FormDelegateSeparator {
            visible: adminSignUpDelegate.visible
        }

        EventControl {
            id: adminReportDelegate

            text: i18n("Server Reports")
            description: i18n("When someone files a report against a user on your server.")
            visible: root.account.identity.permission & AdminAccountInfo.ManageUsers

            shouldDisplay: root.config.displayReport
            onToggledDisplay: display => root.config.displayReport = display

            notify: root.config.notifyReport
            onToggledNotify: notify => root.config.notifyReport = notify
        }

        FormCard.FormDelegateSeparator {
            visible: adminReportDelegate.visible
        }

        EventControl {
            id: relationshipsDelegate

            text: i18n("Severed Relationships")
            description: i18n("When you or your server moderates another server, which you are following users or have followers with.")

            shouldDisplay: root.config.displayRelationships
            onToggledDisplay: display => root.config.displayRelationships = display

            notify: root.config.notifyRelationships
            onToggledNotify: notify => root.config.notifyRelationships = notify
        }

        FormCard.FormDelegateSeparator {}

        EventControl {
            id: annualReportDelegate

            text: i18nc("@option:check", "Annual Report")
            description: i18n("When you receive your #FediWrapped at the end of the year.")

            shouldDisplay: root.config.displayAnnualReport
            onToggledDisplay: display => root.config.displayAnnualReport = display

            notify: root.config.notifyAnnualReport
            onToggledNotify: notify => root.config.notifyAnnualReport = notify
        }
    }
}
