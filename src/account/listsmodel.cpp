// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "listsmodel.h"

#include "abstractaccount.h"
#include "accountmanager.h"

using namespace Qt::Literals::StringLiterals;

ListsModel::ListsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
}

QVariant ListsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &list = m_lists[index.row()];

    switch (role) {
    case IdRole:
        return list.id;
    case TitleRole:
        return list.title;
    default:
        return {};
    }
}

bool ListsModel::loading() const
{
    return m_loading;
}

void ListsModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int ListsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_lists.size();
}

QHash<int, QByteArray> ListsModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {TitleRole, "title"},
    };
}

void ListsModel::fillTimeline()
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);

    account->get(account->apiUrl(QStringLiteral("/api/v1/lists")), true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        auto lists = doc.array().toVariantList();

        if (!lists.isEmpty()) {
            QList<List> fetchedLists;

            std::transform(lists.cbegin(), lists.cend(), std::back_inserter(fetchedLists), [=](const QVariant &value) -> auto {
                return fromSourceData(value.toJsonObject());
            });
            beginInsertRows({}, m_lists.size(), m_lists.size() + fetchedLists.size() - 1);
            m_lists += fetchedLists;
            endInsertRows();
        }

        setLoading(false);
    });
}

ListsModel::List ListsModel::fromSourceData(const QJsonObject &object) const
{
    List list;
    list.id = object["id"_L1].toString();
    list.title = object["title"_L1].toString();

    return list;
}

#include "moc_listsmodel.cpp"