// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "account/abstractaccount.h"

class AccountWarning
{
    Q_GADGET

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(Action action READ action CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)

public:
    enum Action { None, Disable, MarkStatusesAsSensitive, DeleteStatuses, Sensitive, Silence, Suspend };
    Q_ENUM(Action);

    AccountWarning(const QJsonObject &source);

    QString id() const;
    Action action() const;
    QString text() const;
    QDateTime createdAt() const;

private:
    QString m_id;
    Action m_action;
    QString m_text;
    QDateTime m_createdAt;
};

class RelationshipSeveranceEvent
{
    Q_GADGET

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(bool purged READ purged CONSTANT)
    Q_PROPERTY(QString targetName READ targetName CONSTANT)
    Q_PROPERTY(int relationshipsCount READ relationshipsCount CONSTANT)

public:
    enum Type { DomainBlock, UserDomainBlock, AccountSuspension };
    Q_ENUM(Type);

    RelationshipSeveranceEvent(const QJsonObject &source);

    QString id() const;
    Type type() const;
    bool purged() const;
    QString targetName() const;
    int relationshipsCount() const;
    QDateTime createdAt() const;

private:
    QString m_id;
    Type m_type;
    bool m_purged;
    QString m_targetName;
    int m_followersCount = 0;
    int m_followingCount = 0;
    QDateTime m_createdAt;
};

class Notification
{
    Q_GADGET

public:
    Notification() = default;
    explicit Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent = nullptr);

    enum Type { Mention, Follow, Repeat, Favorite, Poll, FollowRequest, Update, Status, AdminSignUp, AdminReport, SeveredRelationships, ModerationWarning };
    Q_ENUM(Type);

    [[nodiscard]] int id() const;
    [[nodiscard]] AbstractAccount *account() const;
    [[nodiscard]] Type type() const;
    [[nodiscard]] Post *post() const;
    [[nodiscard]] ReportInfo *report() const;
    [[nodiscard]] std::optional<RelationshipSeveranceEvent> relationshipSeveranceEvent() const;
    [[nodiscard]] std::optional<AccountWarning> accountWarning() const;
    [[nodiscard]] std::shared_ptr<Identity> identity() const;

private:
    int m_id = 0;

    AbstractAccount *m_account = nullptr;
    Post *m_post = nullptr;
    ReportInfo *m_report = nullptr;
    std::optional<RelationshipSeveranceEvent> m_relationshipSeveranceEvent;
    std::optional<AccountWarning> m_accountWarning;
    Type m_type = Type::Favorite;
    std::shared_ptr<Identity> m_identity;

    Post *createPost(AbstractAccount *account, const QJsonObject &obj, QObject *parent);
};
