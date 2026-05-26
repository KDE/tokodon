// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>

#include "account/abstractaccount.h"

/**
 * @brief Fetches and displays a single collection.
 */
class CollectionModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString collectionId READ collectionId WRITE setCollectionId NOTIFY collectionIdChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString accountId READ accountId NOTIFY accountIdChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        IdentityRole = Qt::UserRole, /** Identity of the account. */
    };

    explicit CollectionModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QString collectionId() const;
    void setCollectionId(const QString &collectionId);

    QString name() const;
    QString description() const;
    QString accountId() const;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void removeItem(const QModelIndex index);

Q_SIGNALS:
    void loadingChanged();
    void collectionIdChanged();
    void nameChanged();
    void descriptionChanged();
    void accountIdChanged();

private:
    void fill();

    struct Item {
        QString id;
        QString accountId;
    };

    QList<Item> m_items;
    bool m_loading = false;
    AbstractAccount *m_account = nullptr;
    QString m_collectionId;
    QString m_name;
    QString m_description;
    QString m_accountId;
};
