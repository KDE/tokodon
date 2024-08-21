// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "account/abstractaccount.h"
#include "timeline/abstracttimelinemodel.h"

/**
 * @brief Model building on top of AbstractTimelineModel, used by MainTimelineModel and ThreadModel for example.
 * @see AbstractTimelineModel
 */
class TimelineModel : public AbstractTimelineModel
{
    Q_OBJECT

    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)
    Q_PROPERTY(bool shouldLoadMore MEMBER m_shouldLoadMore WRITE setShouldLoadMore NOTIFY shouldLoadMoreChanged)
    Q_PROPERTY(bool showReplies MEMBER m_showReplies NOTIFY showRepliesChanged)
    Q_PROPERTY(bool showBoosts MEMBER m_showBoosts NOTIFY showBoostsChanged)

public:
    explicit TimelineModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief Start filling the timeline starting at @p fromId
     */
    virtual void fillTimeline(const QString &fromId = {}) = 0;

    /**
     * @return A translated label for the timeline (e.g. "Home")
     */
    [[nodiscard]] virtual QString displayName() const = 0;

    /**
     * @brief Handle an incoming streaming event.
     */
    virtual void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

    /**
     * @brief Initialize and start filling the timeline.
     */
    void init();

    void setShouldLoadMore(bool shouldLoadMore);

public Q_SLOTS:
    /**
     * @brief Reply to the post at @p index.
     * @see wantReply()
     */
    void actionReply(const QModelIndex &index);

    /**
     * @brief Favorite the post at @p index.
     */
    void actionFavorite(const QModelIndex &index);

    /**
     * @brief Boost the post at @p index.
     */
    void actionRepeat(const QModelIndex &index);

    /**
     * @brief Vote on the post at @p index.
     */
    void actionVote(const QModelIndex &index, const QList<int> &choices);

    /**
     * @brief Bookmark the post at @p index.
     */
    void actionBookmark(const QModelIndex &index);

    /**
     * @brief Delete & re-draft the post at @p index.
     * @see postSourceReady()
     */
    void actionRedraft(const QModelIndex &index, bool isEdit);

    /**
     * @brief Delete the post at @p index.
     */
    void actionDelete(const QModelIndex &index);

    /**
     * @brief Pin the post at @p index.
     */
    void actionPin(const QModelIndex &index);

    /**
     * @brief Reset the timeline posts, and any additional state.
     */
    virtual void reset() = 0;

Q_SIGNALS:
    /// Emitted when actionReply is called
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);

    /// Emitted when the timeline display name has changed
    void nameChanged();

    void shouldLoadMoreChanged();

    void showRepliesChanged();
    void showBoostsChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;
    void fetchedTimeline(const QByteArray &array, bool alwaysAppendToEnd = false);

    AccountManager *m_manager = nullptr;

    QList<Post *> m_timeline;

    bool m_shouldLoadMore = true;
    bool m_showReplies = true;
    bool m_showBoosts = true;
    friend class TimelineTest;
};
