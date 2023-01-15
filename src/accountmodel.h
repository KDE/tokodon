// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "timelinemodel.h"

class AbstractAccount;

/// TimelineModel to show the last post of an account
///
/// This expose as well some properties of the user (\see identity)
class AccountModel : public TimelineModel
{
    Q_OBJECT

    /// The account id of the account we want to display
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)

    /// The identity of the account
    Q_PROPERTY(Identity *identity READ identity NOTIFY identityChanged)

    /// The account of the current user
    Q_PROPERTY(AbstractAccount *account READ account NOTIFY accountChanged)

    /// This property holds whether the current user is the account displayed by this model
    Q_PROPERTY(bool isSelf READ isSelf NOTIFY identityChanged)

public:
    explicit AccountModel(QObject *parent = nullptr);
    ~AccountModel();

    QString accountId() const;
    void setAccountId(const QString &accountId);

    Identity *identity() const;

    QString displayName() const override;
    AbstractAccount *account() const;
    bool isSelf() const;

    void fillTimeline(const QString &fromId = {}) override;

Q_SIGNALS:
    void identityChanged();
    void accountChanged();
    void accountIdChanged();

private:
    void updateRelationships();

    std::shared_ptr<Identity> m_identity;
    QString m_accountId;
};
