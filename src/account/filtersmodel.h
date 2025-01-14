// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

/**
 * @brief Fetches a user's lists.
 */
class FiltersModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        IdRole = Qt::UserRole, /**< ID of the list. */
        TitleRole /**< Title of the list. */
    };

    explicit FiltersModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void fillTimeline();

Q_SIGNALS:
    void loadingChanged();

private:
    struct Filter {
        QString id;
        QString title;
        QString context;
        std::optional<QDateTime> expiresAt;
        QString filterAction;
    };

    QList<Filter> m_filters;
    bool m_loading = false;
    [[nodiscard]] Filter fromSourceData(const QJsonObject &object) const;
};
