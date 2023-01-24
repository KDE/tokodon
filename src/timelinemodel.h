// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "abstractaccount.h"
#include "abstracttimelinemodel.h"
#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>

class TimelineModel : public AbstractTimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)

public:
    explicit TimelineModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    virtual void fillTimeline(const QString &fromId = {}) = 0;
    virtual QString displayName() const = 0;
    virtual void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

    void init();

public Q_SLOTS:
    void actionReply(const QModelIndex &index);
    void actionFavorite(const QModelIndex &index);
    void actionRepeat(const QModelIndex &index);
    void actionVis(const QModelIndex &index);
    void actionVote(const QModelIndex &index, const QList<int> &choices);
    void actionBookmark(const QModelIndex &index);
    void actionEdit(const QModelIndex &index);

Q_SIGNALS:
    void wantReply(AbstractAccount *account, Post *post, const QModelIndex &index);
    void nameChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchedTimeline(const QByteArray &array);

    AccountManager *m_manager = nullptr;

    QList<Post *> m_timeline;

    friend class TimelineTest;
};
