// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "abstracttimelinemodel.h"
#include "post.h"
#include <QAbstractListModel>

/// Model building on top of AbstractTimelineModel, used by MainTimelineModel and ThreadModel for example
/// \see AbstractTimelineModel
class TimelineModel : public AbstractTimelineModel
{
    Q_OBJECT

    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)
    Q_PROPERTY(bool shouldLoadMore MEMBER m_shouldLoadMore WRITE setShouldLoadMore NOTIFY shouldLoadMoreChanged)

public:
    explicit TimelineModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    /// Start filling the timeline starting at \p fromId
    virtual void fillTimeline(const QString &fromId = {}) = 0;

    /// Get a translated label for the timeline (e.g. "Home")
    virtual QString displayName() const = 0;

    /// Handle an incoming streaming event
    virtual void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

    /// Initialize and start filling the timeline
    void init();

    void setShouldLoadMore(bool shouldLoadMore);

public Q_SLOTS:
    /// Reply to the post at \p index
    /// \see wantReply
    void actionReply(const QModelIndex &index);

    /// Favorite the post at \p index
    void actionFavorite(const QModelIndex &index);

    /// Boost the post at \p index
    void actionRepeat(const QModelIndex &index);

    /// Vote on the post at \p index
    void actionVote(const QModelIndex &index, const QList<int> &choices);

    /// Bookmark the post at \p index
    void actionBookmark(const QModelIndex &index);

    /// Delete & re-draft the post at \p index
    /// \see postSourceReady
    void actionRedraft(const QModelIndex &index, bool isEdit);

    /// Delete the post at \p index
    void actionDelete(const QModelIndex &index);

    /// Pin the post at \p index
    void actionPin(const QModelIndex &index);
Q_SIGNALS:
    /// Emitted when actionReply is called
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);

    /// Emitted when the timeline display name has changed
    void nameChanged();

    void shouldLoadMoreChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchedTimeline(const QByteArray &array, bool alwaysAppendToEnd = false);

    AccountManager *m_manager = nullptr;

    QList<Post *> m_timeline;

    bool m_shouldLoadMore = true;
    friend class TimelineTest;
};
