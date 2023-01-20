// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "abstracttimelinemodel.h"
#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>

class NotificationModel : public AbstractTimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList excludeTypes READ excludeTypes WRITE setExcludesTypes NOTIFY excludeTypesChanged)

public:
    explicit NotificationModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    virtual void fillTimeline(const QUrl &next = {});

    std::shared_ptr<Notification> internalData(const QModelIndex &index) const;
    QStringList excludeTypes() const;
    void setExcludesTypes(const QStringList &excludeTypes);

public Q_SLOTS:
    void actionReply(const QModelIndex &index);
    void actionFavorite(const QModelIndex &index);
    void actionRepeat(const QModelIndex &index);
    void actionVis(const QModelIndex &index);

Q_SIGNALS:
    void excludeTypesChanged();
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QString m_timelineName;
    AccountManager *m_manager = nullptr;
    AbstractAccount *m_account = nullptr;

    QList<std::shared_ptr<Notification>> m_notifications;
    QStringList m_excludeTypes;
    QUrl m_next;
};
