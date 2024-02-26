// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timeline/timelinemodel.h"

class AbstractAccount;

/// TimelineModel to show the last post of an account
///
/// This expose as well some properties of the user (\see identity)
class AccountModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    /// The account id of the account we want to display
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)

    /// The identity of the account
    Q_PROPERTY(Identity *identity READ identity NOTIFY identityChanged)

    /// The account of the current user
    Q_PROPERTY(AbstractAccount *account READ account NOTIFY accountChanged)

    /// This property holds whether the current user is the account displayed by this model
    Q_PROPERTY(bool isSelf READ isSelf NOTIFY identityChanged)

    /// Whether or not to exclude replies
    Q_PROPERTY(bool excludeReplies MEMBER m_excludeReplies NOTIFY filtersChanged)

    /// Whether or not to exclude boosts
    Q_PROPERTY(bool excludeBoosts MEMBER m_excludeBoosts NOTIFY filtersChanged)

    /// Whether or not to exclude pinned posts
    Q_PROPERTY(bool excludePinned MEMBER m_excludePinned NOTIFY filtersChanged)

    /// Only include posts that have a media attachment
    Q_PROPERTY(bool onlyMedia MEMBER m_onlyMedia NOTIFY filtersChanged)

    /// Search for posts with a specific hashtag, leave blank to not search for any
    Q_PROPERTY(QString tagged MEMBER m_tagged NOTIFY filtersChanged)

    /// The current account tab, is set to None by default to disable filtering.
    Q_PROPERTY(AccountTab currentTab MEMBER m_currentTab NOTIFY tabChanged)

public:
    explicit AccountModel(QObject *parent = nullptr);

    enum AccountTab { Posts, Replies, Media, None };
    Q_ENUM(AccountTab)

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
    void filtersChanged();
    void tabChanged();

protected:
    void reset() override;

private:
    void updateRelationships();
    void updateTabFilters();

    std::shared_ptr<Identity> m_identity;
    QString m_accountId;

    bool m_excludeReplies = false;
    bool m_excludeBoosts = false;
    bool m_excludePinned = false;
    bool m_onlyMedia = false;
    QString m_tagged;
    AccountTab m_currentTab = AccountTab::None;
};
