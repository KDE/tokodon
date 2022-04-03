// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "account.h"
#include "timelinemodel.h"

class AccountModel : public TimelineModel
{
    Q_OBJECT
    Q_PROPERTY(Identity *identity READ identity NOTIFY identityChanged)

public:
    AccountModel(AccountManager *manager, int id, const QString &acct, QObject *parent = nullptr);

    QString displayName() const override;
    Identity *identity() const;
    void fillTimeline(const QString &fromId = QString()) override;
    bool canFetchMore(const QModelIndex &parent) const override;
    Q_INVOKABLE void followAccount();
    Q_INVOKABLE void unfollowAccount();

Q_SIGNALS:
    void identityChanged();

private:
    std::shared_ptr<Identity> m_identity;
    int m_id;
};
