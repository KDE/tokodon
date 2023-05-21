// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "post.h"

class QNetworkAccessManager;

class NotificationHandler : public QObject
{
    Q_OBJECT

public:
    explicit NotificationHandler(QNetworkAccessManager *nam, QObject *parent = nullptr);
    void handle(std::shared_ptr<Notification> notification, AbstractAccount *account);

private:
    QNetworkAccessManager *m_nam;
};
