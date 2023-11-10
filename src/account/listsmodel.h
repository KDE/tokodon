// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml>

/// Fetches a user's lists
class ListsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    /// Custom roles for this model
    enum CustomRoles {
        IdRole = Qt::UserRole, ///< ID of the list
        TitleRole ///< Title of the list
    };

    explicit ListsModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    void fillTimeline();

Q_SIGNALS:
    void loadingChanged();

private:
    struct List {
        QString id, title;
    };

    QList<List> m_lists;
    bool m_loading = false;
    List fromSourceData(const QJsonObject &object) const;
};
