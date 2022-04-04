// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "account.h"
#include "timelinemodel.h"
#include <qobjectdefs.h>

class AccountModel : public TimelineModel
{
    Q_OBJECT
    Q_PROPERTY(Identity *identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(Account *account READ account NOTIFY accountChanged)
    Q_PROPERTY(bool isSelf READ isSelf NOTIFY identityChanged)

public:
    AccountModel(AccountManager *manager, qint64 id, const QString &acct, QObject *parent = nullptr);

    QString displayName() const override;
    Account *account() const;
    Identity *identity() const;
    bool isSelf() const;

    void fillTimeline(const QString &fromId = QString()) override;

Q_SIGNALS:
    void identityChanged();
    void accountChanged();

private:
    std::shared_ptr<Identity> m_identity;
    qint64 m_id;
};
