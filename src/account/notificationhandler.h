// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "timeline/notification.h"

class QNetworkAccessManager;

/**
 * @brief Handles desktop notifications using KNotification.
 */
class NotificationHandler : public QObject
{
    Q_OBJECT

public:
    explicit NotificationHandler(QNetworkAccessManager *nam, QObject *parent = nullptr);

    /**
     * @brief Display a new notification for an account.
     * @param notification The shared handle for the notification.
     * @param account The account the notification belongs to.
     */
    void handle(std::shared_ptr<Notification> notification, AbstractAccount *account);

private:
    QNetworkAccessManager *m_nam;
};
