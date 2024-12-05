// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

/**
 * @brief Fetches server announcements.
 */
class AnnouncementModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        IdRole = Qt::UserRole, /**< ID of the announcement. */
        ContentRole, /**< Content of the announcement, given in rich HTML. */
        PublishedAt /**< The date and time the announcement was published. */
    };

    explicit AnnouncementModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void fillTimeline();

Q_SIGNALS:
    void loadingChanged();

private:
    struct Announcement {
        QString id;
        QString content;
        QDateTime publishedAt;
    };

    QList<Announcement> m_announcements;
    bool m_loading = false;
    [[nodiscard]] Announcement fromSourceData(const QJsonObject &object) const;
};
