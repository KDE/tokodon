// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "timeline/timelinemodel.h"

class AbstractAccount;

/**
 * @brief Model for the three main timelines (Home, Public, and Federated)
 * @see TimelineModel
 */
class MainTimelineModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString listId READ listId WRITE setListId NOTIFY listIdChanged)
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)
    Q_PROPERTY(bool hasPrevious READ hasPrevious NOTIFY hasPreviousChanged)
    Q_PROPERTY(QDateTime lastReadTime READ lastReadTime NOTIFY hasPreviousChanged)
    Q_PROPERTY(bool userHasTakenReadAction READ userHasTakenReadAction NOTIFY userHasTakenReadActionChanged)

public:
    explicit MainTimelineModel(QObject *parent = nullptr);

    /**
     * @return Name of the timeline.
     * @see setName()
     */
    [[nodiscard]] QString name() const;

    /**
     * @brief Set the name of the timeline to fetch ("home", "public" or "federated")
     * @param name Can be "home", "public", "federated", "bookmarks", "favourites", "trending" or "list".
     */
    void setName(const QString &name);

    /**
     * @return Name of the list id.
     */
    [[nodiscard]] QString listId() const;

    /**
     * @brief Set the name of the list to view, only works if name is set to "list".
     */
    void setListId(const QString &id);

    void fillTimeline(const QString &fromId, bool backwards = false) override;
    [[nodiscard]] QString displayName() const override;
    void handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] bool atEnd() const;
    [[nodiscard]] bool hasPrevious() const;
    [[nodiscard]] QDateTime lastReadTime() const;
    [[nodiscard]] bool userHasTakenReadAction() const;

    void reset() override;
    bool loading() const override;

    Q_INVOKABLE void fetchPrevious();

Q_SIGNALS:
    void atEndChanged();
    void listIdChanged();
    void repositionAt(int index);
    void hasPreviousChanged();
    void userHasTakenReadActionChanged();

private:
    QString m_timelineName;
    QString m_listId;

    std::optional<QUrl> m_next, m_prev;
    QString m_lastReadId, m_initialLastReadId;
    bool fetchingLastId = false;
    bool fetchedLastId = false;

    void fetchLastReadId();
    QDateTime m_lastReadTime;
    bool m_userHasTakenReadAction = false;
};
