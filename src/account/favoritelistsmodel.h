// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

#include "account/abstractaccount.h"

/**
 * @brief Fetches and displays a user's favorite lists.
 */
class FavoriteListsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        IdRole = Qt::UserRole, /**< ID of the list. */
        NameRole, /**< Name of the list. */
    };

    explicit FavoriteListsModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    AbstractAccount *account() const;
    void setAccount(AbstractAccount *account);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void loadingChanged();
    void accountChanged();

private:
    void reloadLists();

    struct FavoriteList {
        QString id;
        QString name;
    };
    QList<FavoriteList> m_favoritedLists;
    bool m_loading = false;

    AbstractAccount *m_account = nullptr;
};
