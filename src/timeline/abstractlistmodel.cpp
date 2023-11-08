// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "abstractlistmodel.h"

AbstractListModel::AbstractListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_manager = &AccountManager::instance();
    m_account = m_manager->selectedAccount();
}

QString AbstractListModel::name() const
{
    return m_listName;
}

void AbstractListModel::setName(const QString &name)
{
    if (m_listName == name) {
        return;
    }

    setLoading(false);
    m_listName = name;
    Q_EMIT nameChanged();
}

bool AbstractListModel::loading() const
{
    return m_loading;
}

void AbstractListModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

bool AbstractListModel::shouldLoadMore() const
{
    return m_shouldLoadMore;
}

void AbstractListModel::setShouldLoadMore(bool shouldLoadMore)
{
    if (m_shouldLoadMore == shouldLoadMore) {
        return;
    }
    m_shouldLoadMore = shouldLoadMore;
    Q_EMIT shouldLoadMoreChanged();
}

AbstractAccount *AbstractListModel::account() const
{
    return m_account;
}

#include "moc_abstractlistmodel.cpp"