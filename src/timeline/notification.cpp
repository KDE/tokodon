// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "timeline/notification.h"

using namespace Qt::StringLiterals;

Post *Notification::createPost(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
{
    if (!obj.empty()) {
        return new Post(account, obj, parent);
    }

    return nullptr;
}

static QMap<QString, Notification::Type> str_to_not_type = {
    {QStringLiteral("favourite"), Notification::Type::Favorite},
    {QStringLiteral("follow"), Notification::Type::Follow},
    {QStringLiteral("mention"), Notification::Type::Mention},
    {QStringLiteral("reblog"), Notification::Type::Repeat},
    {QStringLiteral("update"), Notification::Type::Update},
    {QStringLiteral("poll"), Notification::Type::Poll},
    {QStringLiteral("status"), Notification::Type::Status},
    {QStringLiteral("follow_request"), Notification::Type::FollowRequest},
    {QStringLiteral("admin.sign_up"), Notification::Type::AdminSignUp},
};

Notification::Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
    : m_account(account)
{
    const auto accountObj = obj["account"_L1].toObject();
    const auto status = obj["status"_L1].toObject();
    const auto accountId = accountObj["id"_L1].toString();
    const auto type = obj["type"_L1].toString();

    m_post = createPost(m_account, status, parent);
    m_identity = m_account->identityLookup(accountId, accountObj);
    m_type = str_to_not_type[type];
    m_id = obj["id"_L1].toString().toInt();
}

int Notification::id() const
{
    return m_id;
}

AbstractAccount *Notification::account() const
{
    return m_account;
}

Notification::Type Notification::type() const
{
    return m_type;
}

Post *Notification::post() const
{
    return m_post;
}

std::shared_ptr<Identity> Notification::identity() const
{
    return m_identity;
}

#include "moc_notification.cpp"
