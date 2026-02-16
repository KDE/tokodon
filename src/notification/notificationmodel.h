// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timeline/abstracttimelinemodel.h"

/**
 * @brief Model for the notifications page.
 * @see AbstractTimelineModel
 */
class NotificationModel : public AbstractTimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QStringList excludeTypes READ excludeTypes WRITE setExcludesTypes NOTIFY excludeTypesChanged)
    Q_PROPERTY(bool fullyRead READ fullyRead NOTIFY readMarkerChanged)

public:
    enum NotificationRoles {
        ReportRole = ExtraRole,
        RelationshipSeveranceEventRole,
        ModerationWarningRole,
        AnnualReportEventRole,
        UnreadRole,
    };

    explicit NotificationModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    virtual void fillTimeline(const QUrl &next = {});

    /// Get a shared pointer to the underlying notification object at \p index
    [[nodiscard]] std::shared_ptr<Notification> internalData(const QModelIndex &index) const;

    /// Returns the list of excluded notification types
    /// \see setExcludesTypes
    [[nodiscard]] QStringList excludeTypes() const;

    /**
     * @brief Set the types of notifications to exclude.
     *
     * Valid options are "mention", "status", "reblog", "follow", "follow_request", "favourite", "poll", "update" and "admin.sign_up".
     *
     * @see excludeTypes()
     */
    void setExcludesTypes(const QStringList &excludeTypes);

    Q_INVOKABLE void markAllNotificationsAsRead();

    bool fullyRead() const;

    /**
     * @return List of exclude types for the All filter, based on the user config.
     */
    Q_INVOKABLE QStringList allExcludeTypes();

public Q_SLOTS:
    /**
     * @brief Reply to the notification at @p index.
     * @see wantReply()
     */
    void actionReply(const QModelIndex &index);

    /**
     * @brief Favorite the notification at @p index.
     */
    void actionFavorite(const QModelIndex &index);

    /**
     * @brief Boost the notification at @p index.
     */
    void actionRepeat(const QModelIndex &index);

    /**
     * @brief Delete and re-draft the notification at @p index.
     * @see postSourceReady()
     */
    void actionRedraft(const QModelIndex &index, bool isEdit);

    /**
     * @brief Delete the notification at @p index.
     */
    void actionDelete(const QModelIndex &index);

    /**
     * @brief Bookmark the notification at @p index.
     */
    void actionBookmark(const QModelIndex &index);

    /**
     * @brief Mute the notification at @p index.
     */
    void actionMute(const QModelIndex &index);

Q_SIGNALS:
    /**
     * @brief Emitted when the list of excluded notification types change
     * @see setExcludesTypes()
     */
    void excludeTypesChanged();

    /**
     * @brief Emitted when actionReply is called.
     */
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);

    void readMarkerChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;
    void fetchLastReadId();

    QString m_timelineName;
    AccountManager *m_manager = nullptr;

    QList<std::shared_ptr<Notification>> m_notifications;
    QStringList m_excludeTypes;
    std::optional<QUrl> m_next;
    bool m_fetchingLastReadId = false;
    bool m_fetchedLastReadId = false;
    QString m_lastReadId;
};
