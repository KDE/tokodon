// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "account/abstractaccount.h"

class Notification
{
    Q_GADGET

public:
    Notification() = default;
    explicit Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent = nullptr);

    enum Type { Mention, Follow, Repeat, Favorite, Poll, FollowRequest, Update, Status, AdminSignUp };
    Q_ENUM(Type);

    [[nodiscard]] int id() const;
    [[nodiscard]] AbstractAccount *account() const;
    [[nodiscard]] Type type() const;
    [[nodiscard]] Post *post() const;
    [[nodiscard]] std::shared_ptr<Identity> identity() const;

private:
    int m_id = 0;

    AbstractAccount *m_account = nullptr;
    Post *m_post = nullptr;
    Type m_type = Type::Favorite;
    std::shared_ptr<Identity> m_identity;

    Post *createPost(AbstractAccount *account, const QJsonObject &obj, QObject *parent);
};
