// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.ki18n
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
            text: KI18n.i18n("Enable notifications for this account")
            description: {
                if (Controller.pushNotificationsAvailable) {
                    if (root.config.enablePushNotifications) {
                        return KI18n.i18n("Notifications can appear even when Tokodon isn't running.");
                    } else {
                        return KI18n.i18n("Push notifications are available but could not be enabled. Please log out and log back in.");
                    }
                } else {
                    return KI18n.i18n("Notifications will only appear when Tokodon is running.");
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
        title: KI18n.i18nc("@title:group", "Filtering Policy")
    }

    component PolicyCombo: FormCard.FormComboBoxDelegate {
        Layout.fillWidth: true

        textRole: "display"
        valueRole: "value"
        model: [
            {
                display: KI18n.i18nc("@info:Option Accept and show this in notifications", "Accept"),
                value: "accept"
            },
            {
                display: KI18n.i18nc("@info:Option Filter and send to the filtered inbox", "Filter"),
                value: "filter"
            },
            {
                display: KI18n.i18nc("@info:Option Ignore completely, do not show notification", "Ignore"),
                value: "drop"
            },
        ]
    }

    FormCard.FormCard {
        PolicyCombo {
            text: KI18n.i18nc("@label Notification preferences", "People you don't follow")
            description: KI18n.i18nc("@label Notification preferences", "Until you manually approve them.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowing)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowing = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: KI18n.i18nc("@label Notification preferences", "People not following you")
            description: KI18n.i18nc("@label Notification preferences", "Including people who have been following you fewer than 3 days.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowers)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forNotFollowers = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: KI18n.i18nc("@label Notification preferences", "New accounts")
            description: KI18n.i18nc("@label Notification preferences", "Created within the past 30 days.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forNewAccounts)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forNewAccounts = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: KI18n.i18nc("@label Notification preferences", "Unsolicited conversations")
            description: KI18n.i18nc("@label Notification preferences", "Filtered unless it's in reply to your own mention or if you follow the sender.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forPrivateMentions)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forPrivateMentions = currentValue
        }
        FormCard.FormDelegateSeparator {}
        PolicyCombo {
            text: KI18n.i18nc("@label Notification preferences", "Moderated accounts")
            description: KI18n.i18nc("@label Notification preferences", "Limited by server moderators.")
            Component.onCompleted: currentIndex = indexOfValue(AccountManager.selectedAccount.notificationFilteringPolicy.forLimitedAccounts)
            onCurrentValueChanged: AccountManager.selectedAccount.notificationFilteringPolicy.forLimitedAccounts = currentValue
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Events")
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
                    text: checked ? KI18n.i18nc("@action:button", "Hide in All") : KI18n.i18nc("@action:button", "Show in All")
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
                    text: checked ? KI18n.i18nc("@action:button", "Disable notifications") : KI18n.i18nc("@action:button", "Enable notifications")
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

            text: KI18n.i18n("Mentions")
            description: KI18n.i18n("When someone mentions you in a new post, or replies to one of your threads.")

            shouldDisplay: root.config.displayMention
            onToggledDisplay: display => root.config.displayMention = display

            notify: root.config.notifyMention
            onToggledNotify: notify => root.config.notifyMention = notify
        }

        FormCard.FormDelegateSeparator {
            below: mentionsDelegate
            above: quotesDelegate
        }

        EventControl {
            id: quotesDelegate

            text: KI18n.i18n("Quotes")
            description: KI18n.i18n("When someone quotes you in a new post")

            shouldDisplay: root.config.displayQuote
            onToggledDisplay: display => root.config.displayQuote = display

            notify: root.config.notifyQuote
            onToggledNotify: notify => root.config.notifyQuote = notify
        }

        FormCard.FormDelegateSeparator {
            below: quotesDelegate
            above: statusesDelegate
        }

        EventControl {
            id: statusesDelegate

            text: KI18n.i18n("Statuses")
            description: KI18n.i18n("When a user you have notifications turned on for makes a new post.")

            shouldDisplay: root.config.displayStatus
            onToggledDisplay: display => root.config.displayStatus = display

            notify: root.config.notifyStatus
            onToggledNotify: notify => root.config.notifyStatus = notify
        }

        FormCard.FormDelegateSeparator {
            below: statusesDelegate
            above: boostsDelegate
        }

        EventControl {
            id: boostsDelegate

            text: KI18n.i18n("Boosts")
            description: KI18n.i18n("When someone boosted one of your posts.")

            shouldDisplay: root.config.displayBoost
            onToggledDisplay: display => root.config.displayBoost = display

            notify: root.config.notifyBoost
            onToggledNotify: notify => root.config.notifyBoost = notify
        }

        FormCard.FormDelegateSeparator {
            below: boostsDelegate
            above: followersDelegate
        }

        EventControl {
            id: followersDelegate

            text: KI18n.i18n("New followers")
            description: KI18n.i18n("When someone follows you.")

            shouldDisplay: root.config.displayFollow
            onToggledDisplay: display => root.config.displayFollow = display

            notify: root.config.notifyFollow
            onToggledNotify: notify => root.config.notifyFollow = notify
        }

        FormCard.FormDelegateSeparator {
            below: followersDelegate
            above: requestsDelegate
        }

        EventControl {
            id: requestsDelegate

            text: KI18n.i18n("New follow requests")
            description: KI18n.i18n("When an account who requires manual approval wants to follow you.")

            shouldDisplay: root.config.displayFollowRequest
            onToggledDisplay: display => root.config.displayFollowRequest = display

            notify: root.config.notifyFollowRequest
            onToggledNotify: notify => root.config.notifyFollowRequest = notify
        }

        FormCard.FormDelegateSeparator {
            below: requestsDelegate
            above: favoritesDelegate
        }

        EventControl {
            id: favoritesDelegate

            text: KI18n.i18n("Favorites")
            description: KI18n.i18n("When a post you made was favorited by another user.")

            shouldDisplay: root.config.displayFavorite
            onToggledDisplay: display => root.config.displayFavorite = display

            notify: root.config.notifyFavorite
            onToggledNotify: notify => root.config.notifyFavorite = notify
        }

        FormCard.FormDelegateSeparator {
            below: favoritesDelegate
            above: pollsDelegate
        }

        EventControl {
            id: pollsDelegate

            text: KI18n.i18n("Polls")
            description: KI18n.i18n("When a poll you voted in has ended.")

            shouldDisplay: root.config.displayPoll
            onToggledDisplay: display => root.config.displayPoll = display

            notify: root.config.notifyPoll
            onToggledNotify: notify => root.config.notifyPoll = notify
        }

        FormCard.FormDelegateSeparator {
            below: quotesDelegate
            above: editsDelegate
        }

        EventControl {
            id: editsDelegate

            text: KI18n.i18n("Edits")
            description: KI18n.i18n("When a post you interacted with was edited by the author.")

            shouldDisplay: root.config.displayUpdate
            onToggledDisplay: display => root.config.displayUpdate = display

            notify: root.config.notifyUpdate
            onToggledNotify: notify => root.config.notifyUpdate = notify
        }

        FormCard.FormDelegateSeparator {
            below: editsDelegate
            above: adminSignUpDelegate.visible ? adminSignUpDelegate : relationshipsDelegate
        }

        EventControl {
            id: adminSignUpDelegate

            text: KI18n.i18n("Server Sign-ups")
            description: KI18n.i18n("When someone signs up to your server.")
            visible: root.account.identity.permission & AdminAccountInfo.ManageUsers

            shouldDisplay: root.config.displaySignup
            onToggledDisplay: display => root.config.displaySignup = display

            notify: root.config.notifySignup
            onToggledNotify: notify => root.config.notifySignup = notify
        }

        FormCard.FormDelegateSeparator {
            below: adminSignUpDelegate
            above: adminReportDelegate
            visible: adminSignUpDelegate.visible
        }

        EventControl {
            id: adminReportDelegate

            text: KI18n.i18n("Server Reports")
            description: KI18n.i18n("When someone files a report against a user on your server.")
            visible: root.account.identity.permission & AdminAccountInfo.ManageUsers

            shouldDisplay: root.config.displayReport
            onToggledDisplay: display => root.config.displayReport = display

            notify: root.config.notifyReport
            onToggledNotify: notify => root.config.notifyReport = notify
        }

        FormCard.FormDelegateSeparator {
            below: adminReportDelegate
            above: relationshipsDelegate
            visible: adminReportDelegate.visible
        }

        EventControl {
            id: relationshipsDelegate

            text: KI18n.i18n("Severed Relationships")
            description: KI18n.i18n("When you or your server moderates another server, which you are following users or have followers with.")

            shouldDisplay: root.config.displayRelationships
            onToggledDisplay: display => root.config.displayRelationships = display

            notify: root.config.notifyRelationships
            onToggledNotify: notify => root.config.notifyRelationships = notify
        }

        FormCard.FormDelegateSeparator {
            below: relationshipsDelegate 
            above: annualReportDelegate
        }

        EventControl {
            id: annualReportDelegate

            text: KI18n.i18nc("@option:check", "Annual Report")
            description: KI18n.i18n("When you receive your #FediWrapped at the end of the year.")

            shouldDisplay: root.config.displayAnnualReport
            onToggledDisplay: display => root.config.displayAnnualReport = display

            notify: root.config.notifyAnnualReport
            onToggledNotify: notify => root.config.notifyAnnualReport = notify
        }

        FormCard.FormDelegateSeparator {
            below: annualReportDelegate
            above: collectionsDelegate
        }

        EventControl {
            id: collectionsDelegate

            text: KI18n.i18nc("@option:check", "Collections")
            description: KI18n.i18n("When you're added to collections or collections were updated.")
            visible: AccountManager.selectedAccount.supportsApiVersion("mastodon", 10)

            shouldDisplay: root.config.displayCollections
            onToggledDisplay: display => root.config.displayCollections = display

            notify: root.config.notifyCollections
            onToggledNotify: notify => root.config.notifyCollections = notify
        }
    }
}
