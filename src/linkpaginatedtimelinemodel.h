// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timelinemodel.h"

class AbstractAccount;

class LinkPaginationTimelineModel : public TimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit LinkPaginationTimelineModel(QObject *parent = nullptr);
    QString name() const;
    void setName(const QString &name);
    void fillTimeline(const QString &fromId = {}) override;
    QString displayName() const override;

private:
    QString m_timelineName;
    QUrl m_next;
};
