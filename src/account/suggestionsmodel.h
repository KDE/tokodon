// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/abstractlistmodel.h"

class SuggestionsModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit SuggestionsModel(QObject *parent = nullptr);
    ~SuggestionsModel() override;

    enum CustomRoles { SourcesRole = Qt::UserRole, IdentityRole };

    enum Source { Featured, Staff, MostFollowed, MostInteractions, SimilarToRecentlyFollowed, FriendsOfFriends };
    Q_ENUM(Source)

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QString displayName() const override;

private:
    void fill();

    struct Suggestion {
        QList<Source> sources;
        Identity *identity;
    };
    QList<Suggestion> m_links;
    [[nodiscard]] Suggestion fromSourceData(const QJsonObject &object) const;
};
