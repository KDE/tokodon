// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

class Identity;

class SocialGraphModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText NOTIFY nameChanged)
    Q_PROPERTY(QString placeholderExplanation READ placeholderExplanation NOTIFY nameChanged)
    Q_PROPERTY(QString placeholderIconName READ placeholderIconName NOTIFY nameChanged)
    Q_PROPERTY(bool isFollowRequest READ isFollowRequest CONSTANT)
    Q_PROPERTY(bool isFollowing READ isFollowing CONSTANT)
    Q_PROPERTY(bool isFollower READ isFollower CONSTANT)
    Q_PROPERTY(bool isList READ isList CONSTANT)
    Q_PROPERTY(bool isMuteList READ isMuteList CONSTANT)
    Q_PROPERTY(bool isBlockList READ isBlockList CONSTANT)

    /**
     * @brief The account id of the account we want to display
     */
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(QString statusId READ statusId WRITE setStatusId NOTIFY statusIdChanged)
    Q_PROPERTY(QString listId READ listId WRITE setListId NOTIFY listIdChanged)
    Q_PROPERTY(int count READ count WRITE setCount)

public:
    enum CustomRoles { IdentityRole = Qt::UserRole + 1, LastStatusAtRole, RelativeTimeRole };

    explicit SocialGraphModel(QObject *parent = nullptr);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QString name() const;
    void setName(const QString &name);
    [[nodiscard]] QString displayName() const;
    [[nodiscard]] QString placeholderText() const;
    [[nodiscard]] QString placeholderExplanation() const;
    [[nodiscard]] QString placeholderIconName() const;
    [[nodiscard]] QString accountId() const;
    void setAccountId(const QString &accountId);
    [[nodiscard]] bool isFollowRequest() const;
    [[nodiscard]] bool isFollowing() const;
    [[nodiscard]] bool isFollower() const;
    [[nodiscard]] bool isList() const;
    [[nodiscard]] bool isMuteList() const;
    [[nodiscard]] bool isBlockList() const;
    [[nodiscard]] QString statusId() const;
    void setStatusId(const QString &statusId);
    [[nodiscard]] int count() const;
    void setCount(int count);
    [[nodiscard]] QString listId() const;
    void setListId(const QString &listId);

public Q_SLOTS:
    void actionAllow(const QModelIndex &index);
    void actionDeny(const QModelIndex &index);
    void actionUnfollow(const QModelIndex &index);
    void actionRemoveFollower(const QModelIndex &index);
    void actionRemoveFromList(const QModelIndex &index);
    void actionAddToList(const QString &accountId);
    void actionUnblock(const QModelIndex &index);
    void actionUnmute(const QModelIndex &index);

Q_SIGNALS:
    void loadingChanged();
    void nameChanged();
    void accountIdChanged();
    void statusIdChanged();
    void listIdChanged();
    void networkErrorOccurred(const QString &message);

protected:
    void fetchMore(const QModelIndex &parent) override;
    [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;

private:
    void fillTimeline();
    void reset();

    QList<std::shared_ptr<Identity>> m_accounts;
    bool m_loading = false;
    std::optional<QUrl> m_next;

    QString m_followListName;
    QString m_accountId;
    QString m_statusId;
    QString m_listId;
    int m_count = 0;
};
