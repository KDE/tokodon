// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "followrequestmodel.h"

#include "identity.h"

#include "abstractaccount.h"
#include "accountmanager.h"
#include "relationship.h"
#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>

FollowRequestModel::FollowRequestModel(QObject *parent)
{
    auto m_account = AccountManager::instance().selectedAccount();

    setLoading(true);

    auto url = m_account->apiUrl("/api/v1/follow_requests");
    m_account->get(url, true, this, [this, m_account](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll());
        const auto accounts = searchResult.array();

        beginResetModel();

        std::transform(
            accounts.cbegin(),
            accounts.cend(),
            std::back_inserter(m_accounts),
            [m_account](const QJsonValue &value) -> auto{
                const auto account = value.toObject();
                return m_account->identityLookup(account["id"].toString(), account);
            });

        endResetModel();

        setLoading(false);
    });
}

QVariant FollowRequestModel::data(const QModelIndex &index, int role) const
{
    const auto identity = m_accounts[index.row()];

    switch (role) {
    case CustomRoles::IdentityRole:
        return QVariant::fromValue<Identity *>(identity.get());
    default:
        return {};
    }
}

int FollowRequestModel::rowCount(const QModelIndex &parent) const
{
    return m_accounts.count();
}

QHash<int, QByteArray> FollowRequestModel::roleNames() const
{
    return {{CustomRoles::IdentityRole, "identity"}};
}

bool FollowRequestModel::loading() const
{
    return m_loading;
}

void FollowRequestModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

void FollowRequestModel::actionAllow(const QModelIndex &index)
{
    auto m_account = AccountManager::instance().selectedAccount();

    auto requestIdentity = m_accounts[index.row()]->id();

    m_account->get(m_account->apiUrl(QString("/api/v1/follow_requests/%1/authorize").arg(requestIdentity)), true, this, [this, index](QNetworkReply *reply) {
        const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

        m_accounts[index.row()]->relationship()->updateFromJson(newRelation);
    });
}

void FollowRequestModel::actionDeny(const QModelIndex &index)
{
    auto m_account = AccountManager::instance().selectedAccount();

    auto requestIdentity = m_accounts[index.row()]->id();

    m_account->get(m_account->apiUrl(QString("/api/v1/follow_requests/%1/deny").arg(requestIdentity)), true, this, [this, index](QNetworkReply *reply) {
        const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

        m_accounts[index.row()]->relationship()->updateFromJson(newRelation);
    });
}
