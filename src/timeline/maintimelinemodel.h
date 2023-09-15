// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QtQml/qqmlregistration.h>

#include "timelinemodel.h"

class AbstractAccount;

/// Model for the three main timelines (Home, Public, and Federated)
/// \see TimelineModel
class MainTimelineModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit MainTimelineModel(QObject *parent = nullptr);

    /// Name of the timeline
    /// \see setName
    QString name() const;

    /// Set the name of the timeline to fetch ("home", "public" or "federated")
    void setName(const QString &name);

    void fillTimeline(const QString &fromId) override;
    QString displayName() const override;
    void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload) override;

private:
    QString m_timelineName;
    QUrl m_next;
};
