// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "timelinemodel.h"

class AbstractAccount;

/// Model for the three main timelines (Home, Public, and Federated)
/// \see TimelineModel
class MainTimelineModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)

public:
    explicit MainTimelineModel(QObject *parent = nullptr);

    /// Name of the timeline
    /// \see setName
    QString name() const;

    /// Set the name of the timeline to fetch ("home", "public" or "federated")
    /// \p name Can be "home", "public", "federated", "bookmarks", "favourites", "trending" or "list"
    void setName(const QString &name);

    /// Name of the list id
    QString listId() const;

    /// Set the name of the list to view, only works if name is set to "list"
    void setListId(const QString &id);

    void fillTimeline(const QString &fromId) override;
    QString displayName() const override;
    void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload) override;

    bool atEnd() const;

    void reset() override;

Q_SIGNALS:
    void atEndChanged();

private:
    QString m_timelineName;
    QString m_listId;
    QUrl m_next;
};
