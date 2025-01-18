// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

/**
 * @brief Fetches a user's blocked domains.
 */
class BlockedDomainModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        DomainRole = Qt::UserRole, /**< The domain name. */
    };

    explicit BlockedDomainModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void loadDomains();

public Q_SLOTS:
    void actionUnblock(const QModelIndex &index);

Q_SIGNALS:
    void loadingChanged();

private:
    QList<QString> m_domains;
    bool m_loading = false;
};
