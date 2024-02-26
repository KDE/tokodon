// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml>

class Identity;

class SocialGraphModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText NOTIFY nameChanged)
    Q_PROPERTY(bool isFollowRequest READ isFollowRequest CONSTANT)
    Q_PROPERTY(bool isFollowing READ isFollowing CONSTANT)
    Q_PROPERTY(bool isFollower READ isFollower CONSTANT)

    /**
     * @brief The account id of the account we want to display
     */
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(QString statusId READ statusId WRITE setStatusId NOTIFY statusIdChanged)
    Q_PROPERTY(int count READ count WRITE setCount)

public:
    enum CustomRoles {
        IdentityRole = Qt::UserRole + 1,
    };

    explicit SocialGraphModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;
    void setLoading(bool loading);

    QString name() const;
    void setName(const QString &name);
    QString displayName() const;
    QString placeholderText() const;
    QString accountId() const;
    void setAccountId(const QString &accountId);
    bool isFollowRequest() const;
    bool isFollowing() const;
    bool isFollower() const;
    QString statusId() const;
    void setStatusId(const QString &statusId);
    int count() const;
    void setCount(int count);

public Q_SLOTS:
    void actionAllow(const QModelIndex &index);
    void actionDeny(const QModelIndex &index);

Q_SIGNALS:
    void loadingChanged();
    void nameChanged();
    void accountIdChanged();
    void statusIdChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

private:
    void fillTimeline();

    QList<std::shared_ptr<Identity>> m_accounts;
    bool m_loading = false;
    QUrl m_next;

    QString m_followListName;
    QString m_accountId;
    QString m_statusId;
    int m_count = 0;
};