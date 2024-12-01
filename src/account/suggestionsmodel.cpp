// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "account/suggestionsmodel.h"

#include "networkcontroller.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

SuggestionsModel::SuggestionsModel(QObject *parent)
    : AbstractListModel(parent)
{
    fill();
}

SuggestionsModel::~SuggestionsModel() = default;

QVariant SuggestionsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &link = m_links[index.row()];

    switch (role) {
    case SourcesRole:
        return QVariant::fromValue(link.sources);
    case IdentityRole:
        return QVariant::fromValue(link.identity);
    default:
        return {};
    }
}

int SuggestionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_links.size();
}

QHash<int, QByteArray> SuggestionsModel::roleNames() const
{
    return {{SourcesRole, "sources"}, {IdentityRole, "identity"}};
}

QString SuggestionsModel::displayName() const
{
    return i18nc("@title:window", "Suggested Users");
}

void SuggestionsModel::fill()
{
    if (loading()) {
        return;
    }
    setLoading(true);

    account()->get(
        account()->apiUrl(QStringLiteral("/api/v2/suggestions")),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto suggestions = doc.array().toVariantList();
            std::reverse(suggestions.begin(), suggestions.end());

            if (!suggestions.isEmpty()) {
                QList<Suggestion> fetchedSuggestions;

                std::transform(suggestions.cbegin(), suggestions.cend(), std::back_inserter(fetchedSuggestions), [this](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                beginInsertRows({}, m_links.size(), m_links.size() + fetchedSuggestions.size() - 1);
                m_links += fetchedSuggestions;
                endInsertRows();
            }

            setLoading(false);
        },
        [this](QNetworkReply *reply) {
            setLoading(false);
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

static QMap<QString, SuggestionsModel::Source> str_to_act_type = {
    {QStringLiteral("featured"), SuggestionsModel::Source::Featured},
    {QStringLiteral("staff"), SuggestionsModel::Source::Staff},
    {QStringLiteral("most_followed"), SuggestionsModel::Source::MostFollowed},
    {QStringLiteral("most_interactions"), SuggestionsModel::Source::MostInteractions},
    {QStringLiteral("similar_to_recently_followed"), SuggestionsModel::Source::SimilarToRecentlyFollowed},
    {QStringLiteral("friends_of_friends"), SuggestionsModel::Source::FriendsOfFriends},
};

SuggestionsModel::Suggestion SuggestionsModel::fromSourceData(const QJsonObject &object) const
{
    Suggestion link;
    for (const auto &sourceName : object["sources"_L1].toArray()) {
        link.sources.push_back(str_to_act_type[sourceName.toString()]);
    }
    link.identity = account()->identityLookup(object["account"_L1].toObject()["id"_L1].toString(), object["account"_L1].toObject()).get();

    return link;
}

#include "moc_suggestionsmodel.cpp"
