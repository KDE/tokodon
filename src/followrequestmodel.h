// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <memory>

class Identity;

class FollowRequestModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
public:
    enum CustomRoles { IdentityRole = Qt::UserRole + 1 };

    explicit FollowRequestModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;
    void setLoading(bool loading);

    Q_INVOKABLE void actionAllow(const QModelIndex &index);
    Q_INVOKABLE void actionDeny(const QModelIndex &index);

Q_SIGNALS:
    void loadingChanged();

private:
    QList<std::shared_ptr<Identity>> m_accounts;
    bool m_loading = false;
};