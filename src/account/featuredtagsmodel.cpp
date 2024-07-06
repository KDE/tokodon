// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "account/featuredtagsmodel.h"

#include "account/accountmanager.h"
#include "account/relationship.h"

using namespace Qt::StringLiterals;

FeaturedTagsModel::FeaturedTagsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QString FeaturedTagsModel::accountId() const
{
    return m_accountId;
}

void FeaturedTagsModel::setAccountId(const QString &accountId)
{
    if (accountId == m_accountId || accountId.isEmpty()) {
        return;
    }
    m_accountId = accountId;
    Q_EMIT accountIdChanged();

    fill();
}

QVariant FeaturedTagsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &tag = m_tags[index.row()];

    switch (role) {
    case NameRole:
        return tag;
    default:
        return {};
    }
}

int FeaturedTagsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_tags.size();
}

QHash<int, QByteArray> FeaturedTagsModel::roleNames() const
{
    return {
        {NameRole, "name"},
    };
}

void FeaturedTagsModel::fill()
{
    const auto account = AccountManager::instance().selectedAccount();

    account->get(
        account->apiUrl(QStringLiteral("/api/v1/accounts/%1/featured_tags").arg(m_accountId)),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto tags = doc.array().toVariantList();

            if (!tags.isEmpty()) {
                QVector<QString> fetchedTags;

                std::transform(tags.cbegin(), tags.cend(), std::back_inserter(fetchedTags), [=](const QVariant &value) -> auto {
                    return value.toJsonObject()["name"_L1].toString();
                });
                beginInsertRows({}, m_tags.size(), m_tags.size() + fetchedTags.size() - 1);
                m_tags += fetchedTags;
                endInsertRows();
            }
        },
        [](const QNetworkReply *reply) {
            Q_UNUSED(reply)
            // Note: we are silently failing here because some servers don't implement this endpoint
            // See BUG: 484475 and https://docs.akkoma.dev/stable-docs/development/API/differences_in_mastoapi_responses/#featured-tags
        });
}

#include "moc_featuredtagsmodel.cpp"