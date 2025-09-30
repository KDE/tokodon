// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/notificationhandler.h"

#include "account/account.h"
#include "account/accountmanager.h"
#include "network/networkcontroller.h"
#include "tokodon_debug.h"

#include <QJsonDocument>
#include <QPainter>

#include <KLocalizedString>
#include <KNotification>
#include <QNetworkReply>

#ifdef HAVE_KIO
#include <KIO/ApplicationLauncherJob>
#endif

using namespace Qt::StringLiterals;

NotificationHandler::NotificationHandler(QNetworkAccessManager *nam, QObject *parent)
    : QObject(parent)
    , m_nam(nam)
{
}

void NotificationHandler::handle(std::shared_ptr<Notification> notification, AbstractAccount *account)
{
    KNotification *knotification;

    const auto addViewPostAction = [this, &knotification, notification] {
#ifdef HAVE_KIO
        auto viewPostAction = knotification->addAction(i18n("View Post"));
        auto defaultAction = knotification->addDefaultAction(i18n("View Post"));

        auto openPost = [=] {
            if (auto post = notification->post(); post != nullptr) {
                auto url = post->url();
                url.setScheme(QStringLiteral("web+ap"));

                auto *job = new KIO::ApplicationLauncherJob(KService::serviceByDesktopName(QStringLiteral("org.kde.tokodon")));
                job->setUrls({url});
                job->start();
            }
        };

        connect(viewPostAction, &KNotificationAction::activated, this, openPost);
        connect(defaultAction, &KNotificationAction::activated, this, openPost);
#endif
    };

    const auto addViewUserAction = [this, &knotification, notification] {
#ifdef HAVE_KIO
        auto viewProfileActions = knotification->addAction(i18n("View Profile"));
        auto defaultAction = knotification->addDefaultAction(i18n("View Profile"));

        auto viewProfile = [=] {
            auto url = notification->identity()->url();
            url.setScheme(QStringLiteral("web+ap"));

            auto *job = new KIO::ApplicationLauncherJob(KService::serviceByDesktopName(QStringLiteral("org.kde.tokodon")));
            job->setUrls({url});
            job->start();
        };

        connect(viewProfileActions, &KNotificationAction::activated, this, viewProfile);
        connect(defaultAction, &KNotificationAction::activated, this, viewProfile);
#endif
    };

    switch (notification->type()) {
    case Notification::Mention:
        if (!AccountManager::instance().testMode() && !account->config()->notifyMention()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("mention"));
        knotification->setTitle(i18n("%1 mentioned you", notification->identity()->displayName()));
        addViewPostAction();
        break;
    case Notification::Status:
        if (!AccountManager::instance().testMode() && !account->config()->notifyStatus()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("status"));
        knotification->setTitle(i18n("%1 wrote a new post", notification->identity()->displayName()));
        addViewPostAction();
        break;
    case Notification::Repeat:
        if (!AccountManager::instance().testMode() && !account->config()->notifyBoost()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("boost"));
        knotification->setTitle(i18n("%1 boosted your post", notification->identity()->displayName()));
        addViewPostAction();
        break;
    case Notification::Follow:
        if (!AccountManager::instance().testMode() && !account->config()->notifyFollow()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("follow"));
        knotification->setTitle(i18n("%1 followed you", notification->identity()->displayName()));
        addViewUserAction();
        break;
    case Notification::FollowRequest:
        if (!AccountManager::instance().testMode() && !account->config()->notifyFollowRequest()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("follow-request"));
        knotification->setTitle(i18n("%1 requested to follow you", notification->identity()->displayName()));
        addViewUserAction();
        break;
    case Notification::Favorite:
        if (!AccountManager::instance().testMode() && !account->config()->notifyFavorite()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("favorite"));
        knotification->setTitle(i18n("%1 favorited your post", notification->identity()->displayName()));
        addViewPostAction();
        break;
    case Notification::Poll:
        if (!AccountManager::instance().testMode() && !account->config()->notifyPoll()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("poll"));
        knotification->setTitle(i18n("Poll by %1 has ended", notification->identity()->displayName()));
        addViewPostAction();
        break;
    case Notification::Update:
        if (!AccountManager::instance().testMode() && !account->config()->notifyUpdate()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("update"));
        knotification->setTitle(i18n("%1 edited a post", notification->identity()->displayName()));
        addViewPostAction();
        break;
    case Notification::AnnualReport:
        if (!AccountManager::instance().testMode() && !account->config()->notifyUpdate()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("annual-report"));
        knotification->setTitle(
            i18nc("@title this is used for notifications, when an annual report is available. It's similar to spotify wrapped, it shows profile stats / it's a "
                  "recap of the year. The variable is the current year e.g. 2024. Please don't translate the hashtag.",
                  "Your %1 #FediWrapped awaits!",
                  notification->annualReportEvent()->year()));
        knotification->setText(i18n("Unveil your year's highlights and memorable moments on Mastodon!"));
        break;
    default:
        knotification = new KNotification(QStringLiteral("other"));
        knotification->setTitle(i18nc("@title", "New Notification"));
        knotification->setText(i18n("You received a new notification."));
        break;
    }

    constexpr std::array notificationsWithPosts =
        {Notification::Mention, Notification::Status, Notification::Repeat, Notification::Favorite, Notification::Poll, Notification::Update};
    if (notification->post() != nullptr && std::ranges::find(notificationsWithPosts, notification->type()) != notificationsWithPosts.end()) {
        if (!notification->post()->spoilerText().isEmpty()) {
            knotification->setText(xi18n("<b>Content Notice</b>: %1", notification->post()->spoilerText()));
        } else if (!notification->post()->content().isEmpty()) {
            knotification->setText(notification->post()->content());
        } else {
            knotification->setText(i18n("This post has no text."));
        }
    }

    constexpr std::array notificationsWithUser = {Notification::Follow, Notification::FollowRequest};
    if (notification->identity() != nullptr && std::ranges::find(notificationsWithUser, notification->type()) != notificationsWithUser.end()) {
        if (notification->identity()->limited()) {
            knotification->setText(i18n("The moderators of your server has limited this user, view this in Tokodon to see their profile."));
        } else if (!notification->identity()->bio().isEmpty()) {
            knotification->setText(notification->identity()->bio());
        } else {
            knotification->setText(i18n("This user doesn't have a description."));
        }
    }

    knotification->setHint(QStringLiteral("x-kde-origin-name"), account->identity()->displayName());

    if (!notification->identity()->avatarUrl().isEmpty() && !notification->identity()->limited()) {
        const auto avatarUrl = notification->identity()->avatarUrl();
        auto request = QNetworkRequest(avatarUrl);
        auto reply = m_nam->get(request);
        connect(reply, &QNetworkReply::finished, this, [reply, knotification]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError) {
                knotification->sendEvent();
                return;
            }
            QPixmap img;
            img.loadFromData(reply->readAll());

            // Handle avatars that are lopsided in one dimension
            const int biggestDimension = std::max(img.width(), img.height());
            const QRect imageRect{0, 0, biggestDimension, biggestDimension};

            QImage roundedImage(imageRect.size(), QImage::Format_ARGB32);
            roundedImage.fill(Qt::transparent);

            QPainter painter(&roundedImage);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.setPen(Qt::NoPen);

            // Fill background for transparent avatars
            painter.setBrush(Qt::white);
            painter.drawRoundedRect(imageRect, imageRect.width(), imageRect.height());

            QBrush brush(img.scaledToHeight(biggestDimension));
            painter.setBrush(brush);
            painter.drawRoundedRect(imageRect, imageRect.width(), imageRect.height());
            painter.end();

            knotification->setPixmap(QPixmap::fromImage(std::move(roundedImage)));
            knotification->sendEvent();
        });
    } else {
        knotification->sendEvent();
    }
}

void NotificationHandler::handlePush(const QByteArray &message)
{
    const QJsonDocument document = QJsonDocument::fromJson(message);
    if (!document.object().contains("notification_type"_L1)) {
        return;
    }

    // Convert their notification type to ours from the notifyrc
    const QString &theirType = document["notification_type"_L1].toString();
    QString ourType;
    if (theirType == "mention"_L1) {
        ourType = QStringLiteral("mention");
    } else if (theirType == "status"_L1) {
        ourType = QStringLiteral("status");
    } else if (theirType == "reblog"_L1) {
        ourType = QStringLiteral("boost");
    } else if (theirType == "follow"_L1) {
        ourType = QStringLiteral("follow");
    } else if (theirType == "follow_request"_L1) {
        ourType = QStringLiteral("follow-request");
    } else if (theirType == "favourite"_L1) {
        ourType = QStringLiteral("favorite");
    } else if (theirType == "poll"_L1) {
        ourType = QStringLiteral("poll");
    } else if (theirType == "update"_L1) {
        ourType = QStringLiteral("update");
    } else {
        qCWarning(TOKODON_LOG) << "Unknown push notification type" << theirType << "falling back to other.";
        ourType = QStringLiteral("other");
    }

    auto knotification = new KNotification(ourType);
    knotification->setTitle(document["title"_L1].toString());
    knotification->setText(document["body"_L1].toString());

    // Load icon if available
    const QString &iconUrl = document["icon"_L1].toString();
    if (!iconUrl.isEmpty()) {
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        auto request = QNetworkRequest(QUrl(iconUrl));
        auto reply = manager->get(request);
        // TODO: de-duplicate code that's also used in normal notifications
        connect(reply, &QNetworkReply::finished, manager, [reply, knotification]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError) {
                knotification->sendEvent();
                return;
            }
            QPixmap img;
            img.loadFromData(reply->readAll());

            // Handle avatars that are lopsided in one dimension
            const int biggestDimension = std::max(img.width(), img.height());
            const QRect imageRect{0, 0, biggestDimension, biggestDimension};

            QImage roundedImage(imageRect.size(), QImage::Format_ARGB32);
            roundedImage.fill(Qt::transparent);

            QPainter painter(&roundedImage);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.setPen(Qt::NoPen);

            // Fill background for transparent avatars
            painter.setBrush(Qt::white);
            painter.drawRoundedRect(imageRect, imageRect.width(), imageRect.height());

            QBrush brush(img.scaledToHeight(biggestDimension));
            painter.setBrush(brush);
            painter.drawRoundedRect(imageRect, imageRect.width(), imageRect.height());
            painter.end();

            knotification->setPixmap(QPixmap::fromImage(std::move(roundedImage)));
            knotification->sendEvent();
        });
    } else {
        knotification->sendEvent();
    }
}

#include "moc_notificationhandler.cpp"
