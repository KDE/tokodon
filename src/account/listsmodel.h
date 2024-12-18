// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

/**
 * @brief Fetches a user's lists.
 */
class ListsModel : public QAbstractListModel
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

    explicit ListsModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void fillTimeline();

Q_SIGNALS:
    void loadingChanged();

private:
    struct List {
        QString id, title;
    };

    QList<List> m_lists;
    bool m_loading = false;
    [[nodiscard]] List fromSourceData(const QJsonObject &object) const;
};
