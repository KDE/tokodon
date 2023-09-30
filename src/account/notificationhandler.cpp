// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notificationhandler.h"
#include "account.h"
#include <KLocalizedString>
#include <KNotification>
#include <QNetworkAccessManager>
#include <QPainter>

NotificationHandler::NotificationHandler(QNetworkAccessManager *nam, QObject *parent)
    : QObject(parent)
    , m_nam(nam)
{
}

void NotificationHandler::handle(std::shared_ptr<Notification> notification, AbstractAccount *account)
{
    KNotification *knotification;

    switch (notification->type()) {
    case Notification::Mention:
        if (!account->config()->notifyMention()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("mention"));
        knotification->setTitle(notification->identity()->displayName());
        break;
    case Notification::Status:
        if (!account->config()->notifyStatus()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("status"));
        knotification->setTitle(i18n("%1 wrote a new post", notification->identity()->displayName()));
        break;
    case Notification::Repeat:
        if (!account->config()->notifyBoost()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("boost"));
        knotification->setTitle(i18n("%1 boosted your post", notification->identity()->displayName()));
        break;
    case Notification::Follow:
        if (!account->config()->notifyFollow()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("follow"));
        knotification->setTitle(i18n("%1 followed you", notification->identity()->displayName()));
        break;
    case Notification::FollowRequest:
        if (!account->config()->notifyFollowRequest()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("follow-request"));
        knotification->setTitle(i18n("%1 requested to follow you", notification->identity()->displayName()));
        break;
    case Notification::Favorite:
        if (!account->config()->notifyFavorite()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("favorite"));
        knotification->setTitle(i18n("%1 favorited your post", notification->identity()->displayName()));
        break;
    case Notification::Poll:
        if (!account->config()->notifyPoll()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("poll"));
        knotification->setTitle(i18n("Poll by %1 has ended", notification->identity()->displayName()));
        break;
    case Notification::Update:
        if (!account->config()->notifyUpdate()) {
            return;
        }
        knotification = new KNotification(QStringLiteral("update"));
        knotification->setTitle(i18n("%1 edited a post", notification->identity()->displayName()));
        break;

    default:
        Q_UNREACHABLE();
    }

    if (notification->post() != nullptr) {
        knotification->setText(notification->post()->content());
    }
    knotification->setHint(QStringLiteral("x-kde-origin-name"), account->identity()->displayName());

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
            painter.drawRoundedRect(imageRect, imageRect.width(),
                                    imageRect.height());
            painter.end();

            knotification->setPixmap(
                QPixmap::fromImage(std::move(roundedImage)));
            knotification->sendEvent();
        });
    } else {
        knotification->sendEvent();
    }
}
