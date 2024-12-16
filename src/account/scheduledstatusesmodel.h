// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/abstractlistmodel.h"

class ScheduledStatusesModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool drafts READ drafts WRITE setDrafts CONSTANT REQUIRED)

public:
    explicit ScheduledStatusesModel(QObject *parent = nullptr);
    ~ScheduledStatusesModel() override;

    enum CustomRoles {
        IdRole = Qt::UserRole,
        ScheduledAtRole,
        PollRole,
        TextRole,
        MediaIdsRole,
        SensitiveRole,
        VisibilityRole,
        SpoilerTextRole,
        InReplyToRole,
    };

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QString displayName() const override;

    bool drafts() const;
    void setDrafts(bool draft);

private:
    void fill();

    struct ScheduledStatus {
        QString id;
        QDateTime scheduledAt;
        QString text;
    };
    QList<ScheduledStatus> m_statuses;
    [[nodiscard]] ScheduledStatus fromSourceData(const QJsonObject &object) const;

    bool m_drafts = false;

    // Most clients use this to mark statuses as "draft" by putting them far into the future
    const int DRAFT_YEAR = 5000;
};
