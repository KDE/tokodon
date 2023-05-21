// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "abstracttimelinemodel.h"
#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>

/// Model for the notifications page
/// \see AbstractTimelineModel
class NotificationModel : public AbstractTimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList excludeTypes READ excludeTypes WRITE setExcludesTypes NOTIFY excludeTypesChanged)

public:
    explicit NotificationModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    virtual void fillTimeline(const QUrl &next = {});

    /// Get a shared pointer to the underlying notification object at \p index
    std::shared_ptr<Notification> internalData(const QModelIndex &index) const;

    /// Returns the list of excluded notification types
    /// \see setExcludesTypes
    QStringList excludeTypes() const;

    /// Set the types of notifications to exclude
    /// Valid options are "mention", "status", "reblog", "follow", "follow_request", "favourite", "poll" and "update"
    /// \see excludeTypes
    void setExcludesTypes(const QStringList &excludeTypes);

public Q_SLOTS:
    /// Reply to the notification at \p index
    /// \see wantReply
    void actionReply(const QModelIndex &index);

    /// Favorite the notification at \p index
    void actionFavorite(const QModelIndex &index);

    /// Boost the notification at \p index
    void actionRepeat(const QModelIndex &index);

    /// Delete and re-draft the notification at \p index
    /// \see postSourceReady
    void actionRedraft(const QModelIndex &index, bool isEdit);

    /// Delete the notification at \p index
    void actionDelete(const QModelIndex &index);

Q_SIGNALS:
    /// Emitted when the list of excluded notification types change
    /// \see setExcludesTypes
    void excludeTypesChanged();

    /// Emitted when actionReply is called
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QString m_timelineName;
    AccountManager *m_manager = nullptr;

    QList<std::shared_ptr<Notification>> m_notifications;
    QStringList m_excludeTypes;
    QUrl m_next;
};
