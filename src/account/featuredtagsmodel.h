// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtQml>

class FeaturedTagsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @brief The account id of the account we want to display.
     */
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        NameRole = Qt::UserRole, /**< Name of the tag. */
    };

    explicit FeaturedTagsModel(QObject *parent = nullptr);

    QString accountId() const;
    void setAccountId(const QString &accountId);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void accountIdChanged();

private:
    void fill();

    QString m_accountId;
    QVector<QString> m_tags;
};
