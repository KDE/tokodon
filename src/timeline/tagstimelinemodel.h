// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QtQml/qqmlregistration.h>

#include "timelinemodel.h"

/// Model used for fetching posts for a specific tag (like searching for #KDE)
/// \see TimelineModel
class TagsTimelineModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString hashtag READ hashtag WRITE setHashtag NOTIFY hashtagChanged)

public:
    explicit TagsTimelineModel(QObject *parent = nullptr);
    ~TagsTimelineModel() override;

    void fillTimeline(const QString &fromId) override;
    QString displayName() const override;

    /// The hashtag the timeline is searching for
    /// \see setHashtag
    QString hashtag() const;

    /// Sets the hashtag the timeline is searching for
    /// Once called, will start re-filling the timeline
    /// \see hashtag
    void setHashtag(const QString &hashtag);

    void reset() override;

Q_SIGNALS:
    /// Emitted if the hashtag is changed
    /// \see setHashtag
    void hashtagChanged();

private:
    QString m_hashtag;
};
