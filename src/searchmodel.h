// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "abstracttimelinemodel.h"

class Identity;
class Post;

class SearchHashtag
{
public:
    explicit SearchHashtag(const QJsonObject &object);

    QString getName() const;

private:
    QString m_name;
};

class SearchModel : public AbstractTimelineModel
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

public:
    enum ResultType {
        Status,
        Account,
        Hashtag,
    };
    Q_ENUM(ResultType);

    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel();

    bool loaded() const;
    void setLoaded(bool loaded);

    Q_INVOKABLE void search(const QString &queryString);
    Q_INVOKABLE QString labelForType(SearchModel::ResultType sectionType);
    Q_INVOKABLE void clear();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

Q_SIGNALS:
    void loadedChanged();

private:
    QList<std::shared_ptr<Identity>> m_accounts;
    QList<Post *> m_statuses;
    QList<SearchHashtag> m_hashtags;
    bool m_loaded = false;
};