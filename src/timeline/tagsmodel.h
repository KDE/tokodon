// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timelinemodel.h"

/// Model used for fetching posts for a specific tag (like searching for #KDE)
/// \see TimelineModel
class TagsModel : public TimelineModel
{
    Q_OBJECT

    Q_PROPERTY(QString hashtag READ hashtag WRITE setHashtag NOTIFY hashtagChanged)

public:
    explicit TagsModel(QObject *parent = nullptr);
    ~TagsModel() override;

    void fillTimeline(const QString &fromId) override;
    QString displayName() const override;

    /// The hashtag the timeline is searching for
    /// \see setHashtag
    QString hashtag() const;

    /// Sets the hashtag the timeline is searching for
    /// Once called, will start re-filling the timeline
    /// \see hashtag
    void setHashtag(const QString &hashtag);

Q_SIGNALS:
    /// Emitted if the hashtag is changed
    /// \see setHashtag
    void hashtagChanged();

private:
    QString m_hashtag;
};
