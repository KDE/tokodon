// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "abstracttimelinemodel.h"
#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>

class TimelineModel : public AbstractTimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)

public:
    explicit TimelineModel(QObject *parent = nullptr);
    TimelineModel(const QString &timelineName, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    virtual void fillTimeline(const QString &fromId = QString());

    void init();

    QString name() const;
    void setName(const QString &name);

    virtual QString displayName() const;

    void disallowUpdates()
    {
        m_last_fetch = time(nullptr) + 3;
    }
    Post *internalData(const QModelIndex &index) const;

    Q_INVOKABLE void refresh();

    bool fetching() const;

public Q_SLOTS:
    void actionReply(const QModelIndex &index);
    void actionFavorite(const QModelIndex &index);
    void actionRepeat(const QModelIndex &index);
    void actionVis(const QModelIndex &index);
    void actionVote(const QModelIndex &index, const QList<int> &choices);

Q_SIGNALS:
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);
    void nameChanged();
    void fetchingChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QString m_timelineName;
    AccountManager *m_manager = nullptr;

    QList<Post *> m_timeline;
    bool m_fetching = false;
    time_t m_last_fetch;

public Q_SLOTS:
    void fetchedTimeline(AbstractAccount *account, const QString &original_name, const QList<Post *> &posts);
};
