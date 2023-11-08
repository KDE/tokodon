// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtQml/qqmlregistration.h>

#include "timelinemodel.h"

/// Model for displaying and organizing post threads
/// \see TimelineModel
class ThreadModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString postId READ postId WRITE setPostId NOTIFY postIdChanged)

public:
    explicit ThreadModel(QObject *parent = nullptr);

    /// The post id of the "root" post of the thread
    /// \see setPostId
    QString postId() const;

    /// Set the post id of the "root" post of the thread
    /// \see postId
    void setPostId(const QString &postId);

    QVariant data(const QModelIndex &index, int role) const override;

    QString displayName() const override;
    void fillTimeline(const QString &fromId = QString()) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    /// Returns the index of the root post in the model
    /// Can be used to find where exactly to position the view
    Q_INVOKABLE int getRootIndex() const;

    void reset() override;

Q_SIGNALS:
    void postIdChanged();

private:
    QString m_postId;

    friend class TimelineTest;
};
