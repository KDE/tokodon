// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QQmlEngine>

class PublicServersModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum Roles : int {
        DomainRole = Qt::UserRole,
        DescriptionRole,
        IconRole,
        IsPublicRole,
    };

    explicit PublicServersModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;

Q_SIGNALS:
    void loadingChanged();

private:
    void fill();
    void setLoading(bool loading);

    bool m_loading = false;

    struct PublicServer {
        QString domain;
        QString description;
        QString thumbnailUrl;
    };

    QList<PublicServer> m_servers;
    PublicServer fromSourceData(const QJsonObject &object) const;
    QNetworkAccessManager m_netManager;
};
