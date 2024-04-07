// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "search/searchmodel.h"

#include "account/account.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

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
    beginResetModel();
    clear();
    endResetModel();

    auto url = m_account->apiUrl(QStringLiteral("/api/v2/search"));
    url.setQuery({{QStringLiteral("q"), queryString}, {QStringLiteral("resolve"), QStringLiteral("true")}});
    setLoading(true);
    setLoaded(false);
    m_account->get(url, true, this, [this](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();
        const auto statuses = searchResult[QStringLiteral("statuses")].toArray();

        qInfo() << searchResult;

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
                return m_account->identityLookup(account["id"_L1].toString(), account);
            });
        const auto hashtags = searchResult[QStringLiteral("hashtags")].toArray();
        std::transform(
            hashtags.cbegin(),
            hashtags.cend(),
            std::back_inserter(m_hashtags),
            [](const QJsonValue &value) -> auto{ return SearchHashtag(value.toObject()); });
        endResetModel();
        setLoading(false);
        setLoaded(true);
    });
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_accounts.count() + m_statuses.count() + m_hashtags.count();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();

    const bool isStatus = row >= m_accounts.count() && row < m_accounts.count() + m_statuses.size();
    const bool isHashtag = row >= m_accounts.size() + m_statuses.count();

    if (role == TypeRole) {
        if (isHashtag) {
            return Hashtag;
        } else if (isStatus) {
            return Status;
        } else {
            return Account;
        }
    }

    if (isStatus) {
        const auto post = m_statuses[row - m_accounts.count()];
        return postData(post, role);
    }

    if (isHashtag) {
        const auto hashtag = m_hashtags[row - m_accounts.count() - m_statuses.count()];
        switch (role) {
        case IdRole:
            return hashtag.getName();
        }
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
    m_hashtags.clear();
    setLoading(false);
    setLoaded(false);
}

QString SearchModel::labelForType(ResultType sectionType)
{
    switch (sectionType) {
    case Account:
        return i18n("Users");
    case Hashtag:
        return i18n("Hashtags");
    case Status:
        return i18n("Posts");
    default:
        return {};
    }
}

bool SearchModel::loaded() const
{
    return m_loaded;
}

void SearchModel::setLoaded(bool loaded)
{
    if (m_loaded == loaded) {
        return;
    }
    m_loaded = loaded;
    Q_EMIT loadedChanged();
}

SearchHashtag::SearchHashtag(const QJsonObject &object)
{
    m_name = object["name"_L1].toString();
}

QString SearchHashtag::getName() const
{
    return m_name;
}

#include "moc_searchmodel.cpp"
