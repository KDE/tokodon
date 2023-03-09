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
    fillTimeline();
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

    m_account->post(m_account->apiUrl(QString("/api/v1/follow_requests/%1/authorize").arg(requestIdentity)),
                    QJsonDocument{},
                    true,
                    this,
                    [this, index](QNetworkReply *reply) {
                        const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

                        auto relationship = new Relationship(m_accounts[index.row()].get(), newRelation);
                        m_accounts[index.row()]->setRelationship(relationship);

                        beginRemoveRows(QModelIndex(), index.row(), index.row());
                        m_accounts.removeAt(index.row());
                        endRemoveRows();
                    });
}

void FollowRequestModel::actionDeny(const QModelIndex &index)
{
    auto m_account = AccountManager::instance().selectedAccount();

    auto requestIdentity = m_accounts[index.row()]->id();

    m_account->post(m_account->apiUrl(QString("/api/v1/follow_requests/%1/reject").arg(requestIdentity)),
                    QJsonDocument{},
                    true,
                    this,
                    [this, index](QNetworkReply *reply) {
                        const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

                        auto relationship = new Relationship(m_accounts[index.row()].get(), newRelation);
                        m_accounts[index.row()]->setRelationship(relationship);

                        beginRemoveRows(QModelIndex(), index.row(), index.row());
                        m_accounts.removeAt(index.row());
                        endRemoveRows();
                    });
}

bool FollowRequestModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_next.isEmpty();
}

void FollowRequestModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    fillTimeline();
}

void FollowRequestModel::fillTimeline()
{
    auto m_account = AccountManager::instance().selectedAccount();

    setLoading(true);

    QUrl url;
    if (m_next.isEmpty()) {
        url = m_account->apiUrl("/api/v1/follow_requests");
    } else {
        url = m_next;
    }

    m_account->get(url, true, this, [this, m_account](QNetworkReply *reply) {
        const auto followRequestResult = QJsonDocument::fromJson(reply->readAll());
        const auto accounts = followRequestResult.array();

        if (!accounts.isEmpty()) {
            static QRegularExpression re("<(.*)>; rel=\"next\"");
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(next);
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }

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
        }

        setLoading(false);
    });
}
