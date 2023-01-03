// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notificationhandler.h"
#include "account.h"
#include <KLocalizedString>
#include <KNotification>
#include <QNetworkAccessManager>

NotificationHandler::NotificationHandler(QNetworkAccessManager *nam, QObject *parent)
    : QObject(parent)
    , m_nam(nam)
{
}

void NotificationHandler::handle(std::shared_ptr<Notification> notification, AbstractAccount *account)
{
    KNotification *knotification;

    switch (notification->type()) {
    case Notification::Favorite:
        knotification = new KNotification("favorite");
        knotification->setTitle(i18n("%1 favorited your post", notification->identity()->displayName()));
        break;
    case Notification::Follow:
        knotification = new KNotification("follow");
        knotification->setTitle(i18n("%1 followed you", notification->identity()->displayName()));
        break;
    case Notification::Repeat:
        knotification = new KNotification("boost");
        knotification->setTitle(i18n("%1 boosted your post", notification->identity()->displayName()));
        break;
    case Notification::Mention:
        knotification = new KNotification("mention");
        knotification->setTitle(notification->identity()->displayName());
        break;
    case Notification::Update:
        knotification = new KNotification("update");
        knotification->setTitle(i18n("%1 edited a post", notification->identity()->displayName()));
        break;
    default:
        Q_UNREACHABLE();
    }

    knotification->setText(notification->post()->content());
    knotification->setHint(QStringLiteral("x-kde-origin-name"), account->identity().displayName());

    if (!notification->account()->identity().avatarUrl().isEmpty()) {
        const auto avatarUrl = notification->account()->identity().avatarUrl();
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
            knotification->setPixmap(img);
            knotification->sendEvent();
        });
    } else {
        knotification->sendEvent();
    }
}
