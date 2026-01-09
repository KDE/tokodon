// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "timeline/notification.h"

#include "tokodon_debug.h"

using namespace Qt::StringLiterals;

Post *Notification::createPost(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
{
    if (!obj.empty()) {
        return new Post(account, obj, parent);
    }

    return nullptr;
}

static QMap<QString, AccountWarning::Action> str_to_act_type = {
    {QStringLiteral("none"), AccountWarning::Action::None},
    {QStringLiteral("disable"), AccountWarning::Action::Disable},
    {QStringLiteral("mark_statuses_as_sensitive"), AccountWarning::Action::MarkStatusesAsSensitive},
    {QStringLiteral("delete_statuses"), AccountWarning::Action::DeleteStatuses},
    {QStringLiteral("sensitive"), AccountWarning::Action::Sensitive},
    {QStringLiteral("silence"), AccountWarning::Action::Silence},
    {QStringLiteral("suspend"), AccountWarning::Action::Suspend},
};

AccountWarning::AccountWarning(const QJsonObject &source)
{
    m_id = source["id"_L1].toString();
    m_action = str_to_act_type[source["action"_L1].toString()];
    m_text = source["text"_L1].toString();
    m_createdAt = QDateTime::fromString(source["created_at"_L1].toString(), Qt::ISODate).toLocalTime();
}

QString AccountWarning::id() const
{
    return m_id;
}

AccountWarning::Action AccountWarning::action() const
{
    return m_action;
}

QString AccountWarning::text() const
{
    return m_text;
}

QDateTime AccountWarning::createdAt() const
{
    return m_createdAt;
}

static QMap<QString, RelationshipSeveranceEvent::Type> str_to_sev_type = {
    {QStringLiteral("domain_block"), RelationshipSeveranceEvent::Type::DomainBlock},
    {QStringLiteral("user_domain_block"), RelationshipSeveranceEvent::Type::UserDomainBlock},
    {QStringLiteral("account_suspension"), RelationshipSeveranceEvent::Type::AccountSuspension},
};

RelationshipSeveranceEvent::RelationshipSeveranceEvent(const QJsonObject &source)
{
    m_id = source["id"_L1].toString();
    m_type = str_to_sev_type[source["type"_L1].toString()];
    m_purged = source["purged"_L1].toBool();
    m_targetName = source["target_name"_L1].toString();
    m_followersCount = source["followers_count"_L1].toInt();
    m_followingCount = source["following_count"_L1].toInt();
    m_createdAt = QDateTime::fromString(source["created_at"_L1].toString(), Qt::ISODate).toLocalTime();
}

QString RelationshipSeveranceEvent::id() const
{
    return m_id;
}

RelationshipSeveranceEvent::Type RelationshipSeveranceEvent::type() const
{
    return m_type;
}

bool RelationshipSeveranceEvent::purged() const
{
    return m_purged;
}

QString RelationshipSeveranceEvent::targetName() const
{
    return m_targetName;
}

int RelationshipSeveranceEvent::followersCount() const
{
    return m_followersCount;
}

int RelationshipSeveranceEvent::followingCount() const
{
    return m_followingCount;
}

QDateTime RelationshipSeveranceEvent::createdAt() const
{
    return m_createdAt;
}

AnnualReportEvent::AnnualReportEvent() = default;

AnnualReportEvent::AnnualReportEvent(const QJsonObject &source)
    : m_year(source["year"_L1].toString())
{
}

QString AnnualReportEvent::year() const
{
    return m_year;
}

static QMap<QString, Notification::Type> str_to_not_type = {
    {QStringLiteral("favourite"), Notification::Type::Favorite},
    {QStringLiteral("follow"), Notification::Type::Follow},
    {QStringLiteral("mention"), Notification::Type::Mention},
    {QStringLiteral("reblog"), Notification::Type::Repeat},
    {QStringLiteral("update"), Notification::Type::Update},
    {QStringLiteral("poll"), Notification::Type::Poll},
    {QStringLiteral("status"), Notification::Type::Status},
    {QStringLiteral("follow_request"), Notification::Type::FollowRequest},
    {QStringLiteral("admin.sign_up"), Notification::Type::AdminSignUp},
    {QStringLiteral("admin.report"), Notification::Type::AdminReport},
    {QStringLiteral("severed_relationships"), Notification::Type::SeveredRelationships},
    {QStringLiteral("moderation_warning"), Notification::Type::ModerationWarning},
    {QStringLiteral("annual_report"), Notification::Type::AnnualReport},
    {QStringLiteral("quote"), Notification::Type::Quote},
};

Notification::Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
    : m_account(account)
{
    const auto accountObj = obj["account"_L1].toObject();
    const auto status = obj["status"_L1].toObject();
    const auto accountId = accountObj["id"_L1].toString();
    const auto type = obj["type"_L1].toString();

    m_post = createPost(m_account, status, parent);
    m_identity = m_account->identityLookup(accountId, accountObj);
    if (str_to_not_type.contains(type)) {
        m_type = str_to_not_type[type];
    } else {
        qCWarning(TOKODON_LOG) << "Unknown notification type:" << type;
    }
    m_id = obj["id"_L1].toString().toInt();
    m_createdAt = QDateTime::fromString(obj["created_at"_L1].toString(), Qt::ISODate).toLocalTime();

    if (m_type == ModerationWarning) {
        m_accountWarning = AccountWarning(obj["event"_L1].toObject());
    } else if (m_type == SeveredRelationships) {
        m_relationshipSeveranceEvent = RelationshipSeveranceEvent(obj["event"_L1].toObject());
    } else if (m_type == AdminReport) {
        m_report = new ReportInfo();
        m_report->fromSourceData(obj["event"_L1].toObject());
    } else if (m_type == AnnualReport) {
        m_annualReportEvent = AnnualReportEvent(obj["annual_report"_L1].toObject());
    }
}

int Notification::id() const
{
    return m_id;
}

AbstractAccount *Notification::account() const
{
    return m_account;
}

Notification::Type Notification::type() const
{
    return m_type;
}

Post *Notification::post() const
{
    return m_post;
}

ReportInfo *Notification::report() const
{
    return m_report;
}

std::optional<RelationshipSeveranceEvent> Notification::relationshipSeveranceEvent() const
{
    return m_relationshipSeveranceEvent;
}

std::optional<AnnualReportEvent> Notification::annualReportEvent() const
{
    return m_annualReportEvent;
}

std::optional<AccountWarning> Notification::accountWarning() const
{
    return m_accountWarning;
}

std::shared_ptr<Identity> Notification::identity() const
{
    return m_identity;
}

QDateTime Notification::createdAt() const
{
    return m_createdAt;
}

#include "moc_notification.cpp"
