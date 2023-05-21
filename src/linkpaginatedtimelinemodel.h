// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timelinemodel.h"

class AbstractAccount;

/// Model designed for the new type of pagination used in newer timelines, such as the bookmarks timeline
/// \see TimelineModel
class LinkPaginationTimelineModel : public TimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit LinkPaginationTimelineModel(QObject *parent = nullptr);

    /// Name of the timeline
    /// \see setName
    QString name() const;

    /// Set the name of the timeline to fetch ("bookmarks", "favorites" or "trending")
    void setName(const QString &name);

    void fillTimeline(const QString &fromId = {}) override;
    QString displayName() const override;

private:
    QString m_timelineName;
    QUrl m_next;
};
