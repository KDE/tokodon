// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timelinemodel.h"

class TagsModel : public TimelineModel
{
    Q_OBJECT

    Q_PROPERTY(QString hashtag READ hashtag WRITE setHashtag NOTIFY hashtagChanged)

public:
    TagsModel(QObject *parent = nullptr);
    ~TagsModel();

    void fillTimeline(const QString &fromId) override;
    QString displayName() const override;

    QString hashtag() const;
    void setHashtag(const QString &hashtag);

Q_SIGNALS:
    void hashtagChanged();

private:
    QString m_hashtag;
};
