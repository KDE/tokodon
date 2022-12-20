// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "abstracttimelinemodel.h"

class Identity;
class Post;

class SearchModel : public AbstractTimelineModel
{
    Q_OBJECT

public:
    enum ResultType {
        Status,
        Account,
        Hashtag,
    };
    Q_ENUM(ResultType);

    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel();

    Q_INVOKABLE void search(const QString &queryString);
    Q_INVOKABLE QString labelForType(ResultType sectionType);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    void clear();
    QList<std::shared_ptr<Identity>> m_accounts;
    QList<Post *> m_statuses;
};