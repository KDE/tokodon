// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/timelinemodel.h"

/**
 * @brief Model used for fetching posts for a specific tag (like searching for #KDE)
 * @see TimelineModel
 */
class TagsTimelineModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString hashtag READ hashtag WRITE setHashtag NOTIFY hashtagChanged)

public:
    explicit TagsTimelineModel(QObject *parent = nullptr);
    ~TagsTimelineModel() override;

    void fillTimeline(const QString &fromId) override;
    [[nodiscard]] QString displayName() const override;

    /**
     * @brief The hashtag the timeline is searching for.
     * @see setHashtag()
     */
    [[nodiscard]] QString hashtag() const;

    /**
     * @brief Sets the hashtag the timeline is searching for.
     * @see hashtag()
     */
    void setHashtag(const QString &hashtag);

    void reset() override;

Q_SIGNALS:
    /**
     * @brief Emitted if the hashtag is changed
     * @see setHashtag()
     */
    void hashtagChanged();

private:
    QString m_hashtag;
};
