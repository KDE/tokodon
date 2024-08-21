// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "timeline/abstracttimelinemodel.h"

class Identity;
class Post;

/**
 * @brief Represents a search hashtag result, which only stores the name (and skips other information like occurrences) for now.
 * @see SearchModel
 */
class SearchHashtag
{
public:
    explicit SearchHashtag(const QJsonObject &object);

    [[nodiscard]] QString getName() const;

private:
    QString m_name;
};

/**
 * @brief Model used to fetch search results.
 * @see AbstractTimelineModel
 */
class SearchModel : public AbstractTimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

public:
    /**
     * The search result type.
     */
    enum ResultType {
        Status, /** A status (for full-text search) */
        Account, /** An account. */
        Hashtag, /** A hashtag. */
    };
    Q_ENUM(ResultType);

    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel() override;

    /**
     * @return If the search has finished loading
     * @see setLoaded()
     */
    [[nodiscard]] bool loaded() const;

    /**
     * @brief Sets the search loading status
     * @see loaded()
     */
    void setLoaded(bool loaded);

    ///
    /**
     * @brief Start searching for @p queryString.
     */
    Q_INVOKABLE void search(const QString &queryString);

    /**
     * @brief Get a localized label for a result type.
     */
    Q_INVOKABLE QString labelForType(SearchModel::ResultType sectionType);

    /**
     * @brief Clear the fetched search results.
     */
    Q_INVOKABLE void clear();

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

Q_SIGNALS:
    /**
     * @brief Emitted if the loading status has changed.
     * @see setLoaded()
     */
    void loadedChanged();

private:
    QList<std::shared_ptr<Identity>> m_accounts;
    QList<Post *> m_statuses;
    QList<SearchHashtag> m_hashtags;
    bool m_loaded = false;
};