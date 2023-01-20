// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "searchmodel.h"
#include "abstractaccount.h"
#include "account.h"
#include "accountmodel.h"
#include <KLocalizedString>
#include <QNetworkReply>
#include <QUrlQuery>
#include <algorithm>

SearchModel::SearchModel(QObject *parent)
    : AbstractTimelineModel(parent)
{
    m_account = AccountManager::instance().selectedAccount();

    connect(&AccountManager::instance(), &AccountManager::invalidated, this, [=](AbstractAccount *account) {
        if (m_account == account) {
            beginResetModel();
            clear();
            endResetModel();
        }
    });

    connect(&AccountManager::instance(), &AccountManager::accountSelected, this, [=](AbstractAccount *account) {
        if (m_account != account) {
            m_account = account;
            beginResetModel();
            clear();
            endResetModel();
        }
    });
}

SearchModel::~SearchModel() = default;

void SearchModel::search(const QString &queryString)
{
    auto url = m_account->apiUrl("/api/v2/search");
    url.setQuery({{"q", queryString}});
    m_account->get(url, true, this, [this](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();
        const auto statuses = searchResult[QStringLiteral("statuses")].toArray();

        beginResetModel();
        clear();

        std::transform(
            statuses.cbegin(),
            statuses.cend(),
            std::back_inserter(m_statuses),
            [this](const QJsonValue &value) -> auto{ return new Post(m_account, value.toObject(), this); });
        const auto accounts = searchResult[QStringLiteral("accounts")].toArray();
        std::transform(
            accounts.cbegin(),
            accounts.cend(),
            std::back_inserter(m_accounts),
            [this](const QJsonValue &value) -> auto{
                const auto account = value.toObject();
                return m_account->identityLookup(account["id"].toString(), account);
            });
        endResetModel();
    });
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_accounts.count() + m_statuses.count();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();
    const bool isStatus = row >= m_accounts.count();

    if (role == TypeRole) {
        return isStatus ? Status : Account;
    }

    if (isStatus) {
        const auto post = m_statuses[row - m_accounts.count()];
        return postData(post, role);
    }

    const auto identity = m_accounts[row];
    switch (role) {
    case AuthorIdentityRole:
        return QVariant::fromValue<Identity *>(identity.get());
    }

    return {};
}

void SearchModel::clear()
{
    m_accounts.clear();
    qDeleteAll(m_statuses);
    m_statuses.clear();
}

QString SearchModel::labelForType(ResultType sectionType)
{
    switch (sectionType) {
    case Account:
        return i18n("People");
    case Hashtag:
        return i18n("Hashtags");
    case Status:
        return i18n("Post");
    default:
        return {};
    }
}
