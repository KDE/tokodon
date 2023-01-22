// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timelinemodel.h"

class ThreadModel : public TimelineModel
{
    Q_OBJECT

    Q_PROPERTY(QString postId READ postId WRITE setPostId NOTIFY postIdChanged)

public:
    explicit ThreadModel(QObject *parent = nullptr);

    QString postId() const;
    void setPostId(const QString &postId);
    QVariant data(const QModelIndex &index, int role) const override;

    QString displayName() const override;
    void fillTimeline(const QString &fromId = QString()) override;
    bool canFetchMore(const QModelIndex &parent) const override;

Q_SIGNALS:
    void postIdChanged();

private:
    QString m_postId;

    friend class TimelineTest;
};
