// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/favoritelistsmodel.h"

#include <QJsonDocument>
#include <QNetworkReply>

#include "account/accountmanager.h"

using namespace Qt::Literals::StringLiterals;

FavoriteListsModel::FavoriteListsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant FavoriteListsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &list = m_favoritedLists[index.row()];

    switch (role) {
    case IdRole:
        return list.id;
    case NameRole:
        return list.name;
    default:
        return {};
    }
}

bool FavoriteListsModel::loading() const
{
    return m_loading;
}

void FavoriteListsModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

AbstractAccount *FavoriteListsModel::account() const
{
    return m_account;
}

void FavoriteListsModel::setAccount(AbstractAccount *account)
{
    if (m_account != account) {
        if (m_account != nullptr) {
            disconnect(m_account, &AbstractAccount::favoriteListsChanged, this, &FavoriteListsModel::reloadLists);
        }
        m_account = account;
        connect(m_account, &AbstractAccount::favoriteListsChanged, this, &FavoriteListsModel::reloadLists);
        reloadLists();
        Q_EMIT accountChanged();
    }
}

int FavoriteListsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_favoritedLists.size();
}

QHash<int, QByteArray> FavoriteListsModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {NameRole, "name"},
    };
}

void FavoriteListsModel::reloadLists()
{
    beginResetModel();
    m_favoritedLists.clear();
    endResetModel();

    auto config = m_account->config();
    for (auto &list : config->favoriteListIds()) {
        m_account->get(
            m_account->apiUrl(QStringLiteral("/api/v1/lists/%1").arg(list)),
            true,
            this,
            [this, list](QNetworkReply *reply) {
                auto doc = QJsonDocument::fromJson(reply->readAll());

                FavoriteList listObj;
                listObj.id = list;
                listObj.name = doc["title"_L1].toString();

                beginInsertRows({}, m_favoritedLists.size(), m_favoritedLists.size());
                m_favoritedLists.push_back(listObj);
                endInsertRows();
            },
            [this, list](QNetworkReply *reply) {
                // Only remove lists from the favorite list if they are not found
                // We don't want to accidentally wipe it if we encounter a normal network error
                auto doc = QJsonDocument::fromJson(reply->readAll());
                if (doc["error"_L1] == QStringLiteral("Record not found")) {
                    m_account->removeFavoriteList(list);
                }
            });
    }
}

#include "moc_favoritelistsmodel.cpp"
