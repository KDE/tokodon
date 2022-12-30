// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "timelinemodel.h"

class AbstractAccount;

class MainTimelineModel : public TimelineModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit MainTimelineModel(QObject *parent = nullptr);
    QString name() const;
    void setName(const QString &name);
    void fillTimeline(const QString &fromId = {}) override;
    QString displayName() const override;
    void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload) override;

private:
    QString m_timelineName;
};
