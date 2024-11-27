// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/notificationhandler.h"

#include "account/account.h"
#include "account/accountmanager.h"
#include "network/networkcontroller.h"

#include <QPainter>

#include <KLocalizedString>
#include <KNotification>

#ifdef HAVE_KIO
#include <KIO/ApplicationLauncherJob>
#endif

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

    default:
        Q_UNREACHABLE();
    }

    if (notification->post() != nullptr && notification->type() != Notification::Follow && notification->type() != Notification::FollowRequest) {
        if (notification->post()->spoilerText().isEmpty()) {
            knotification->setText(notification->post()->content());
        } else {
            knotification->setText(xi18n("<b>Content Notice</b>: %1", notification->post()->spoilerText()));
        }
    }
    knotification->setHint(QStringLiteral("x-kde-origin-name"), account->identity()->displayName());

    if (m_lastConnection != nullptr) {
        disconnect(m_lastConnection);
    }
    m_lastConnection = connect(knotification, &KNotification::closed, this, &NotificationHandler::lastNotificationClosed);

    if (!notification->identity()->avatarUrl().isEmpty()) {
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

#include "moc_notificationhandler.cpp"
