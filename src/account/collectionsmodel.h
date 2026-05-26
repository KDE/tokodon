// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

/**
 * @brief Fetches a user's collections.
 */
class CollectionsModel : public QAbstractListModel
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
        NameRole /**< Title of the list. */
    };

    explicit CollectionsModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void fillTimeline();

Q_SIGNALS:
    void loadingChanged();

private:
    struct Collection {
        QString id, name;
    };

    QList<Collection> m_collections;
    bool m_loading = false;
    [[nodiscard]] Collection fromSourceData(const QJsonObject &object) const;
};
