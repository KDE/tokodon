// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QDateTime>
#include <QJsonObject>

#include <KLocalizedString>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "reportinfo.h"

using namespace Qt::StringLiterals;

ReportInfo::ReportInfo(QObject *parent)
    : QObject(parent)
    , m_reportStatusList(this, &m_reportStatus)
{
}

QString ReportInfo::reportId() const
{
    return m_reportId;
}

bool ReportInfo::actionTaken() const
{
    return m_actionTaken;
}

void ReportInfo::setActionTaken(bool actionTaken)
{
    if (m_actionTaken == actionTaken) {
        return;
    }
    m_actionTaken = actionTaken;
    Q_EMIT actionTakenUpdated();
}

bool ReportInfo::assignedModerator() const
{
    return m_assignedModerator;
}

void ReportInfo::setAssignedModerator(bool assignedModerator)
{
    if (m_assignedModerator == assignedModerator) {
        return;
    }
    m_assignedModerator = assignedModerator;
    Q_EMIT assignedModeratorUpdated();
}

QDateTime ReportInfo::actionTakenAt() const
{
    return m_actionTakenAt;
}

QString ReportInfo::category() const
{
    return m_category;
}

QString ReportInfo::comment() const
{
    return m_comment;
}

bool ReportInfo::forwarded() const
{
    return m_forwarded;
}

QDateTime ReportInfo::createdAt() const
{
    return m_createdAt;
}

QDateTime ReportInfo::updatedAt() const
{
    return m_updatedAt;
}

int ReportInfo::statusCount() const
{
    return m_reportStatus.length();
}

int ReportInfo::mediaAttachmentCount() const
{
    int count = 0;
    for (auto &post : m_reportStatus) {
        if (!post->attachments().empty()) {
            count += post->attachments().length();
        }
    }
    return count;
}

AdminAccountInfo *ReportInfo::filedAccount() const
{
    return m_filedAccount;
}

AdminAccountInfo *ReportInfo::targetAccount() const
{
    return m_targetAccount;
}

AdminAccountInfo *ReportInfo::assignedAccount() const
{
    return m_assignedAccount;
}

void ReportInfo::setAssignedAccount(AdminAccountInfo *newAssignedAccount)
{
    if (m_assignedAccount == newAssignedAccount) {
        return;
    }
    m_assignedAccount = newAssignedAccount;
    Q_EMIT assignedAccountUpdated();
}

AdminAccountInfo *ReportInfo::actionTakenByAccount() const
{
    return m_actionTakenByAccount;
}

QList<Post *> ReportInfo::reportStatus() const
{
    return m_reportStatus;
}

QQmlListProperty<Post> ReportInfo::reportStatusList() const
{
    return m_reportStatusList;
}

QJsonArray ReportInfo::rules() const
{
    return m_rules;
}

void ReportInfo::reparentReportInfo(AbstractAccount *parent)
{
    m_parent = parent;
}

void ReportInfo::fromSourceData(const QJsonObject &doc)
{
    m_reportId = doc["id"_L1].toString();

    m_actionTaken = doc["action_taken"_L1].toBool();
    m_actionTakenAt = QDateTime::fromString(doc["action_taken_at"_L1].toString(), Qt::ISODate).toLocalTime();
    m_category = doc["category"_L1].toString();
    m_comment = doc["comment"_L1].toString();
    m_forwarded = doc["forwarded"_L1].toBool();
    m_createdAt = QDateTime::fromString(doc["created_at"_L1].toString(), Qt::ISODate).toLocalTime();
    m_updatedAt = QDateTime::fromString(doc["updated_at"_L1].toString(), Qt::ISODate).toLocalTime();

    auto account = AccountManager::instance().selectedAccount();
    const auto filedAccountdoc = doc["account"_L1];
    const auto targetAccountdoc = doc["target_account"_L1];
    const auto assignedAccountdoc = doc["assigned_account"_L1];
    const auto actionTakenByAccountdoc = doc["action_taken_by_account"_L1];
    m_filedAccount = account->adminIdentityLookupWithVanillaPointer(filedAccountdoc["id"_L1].toString(), filedAccountdoc.toObject());
    m_targetAccount = account->adminIdentityLookupWithVanillaPointer(targetAccountdoc["id"_L1].toString(), targetAccountdoc.toObject());
    m_assignedAccount = account->adminIdentityLookupWithVanillaPointer(assignedAccountdoc["id"_L1].toString(), assignedAccountdoc.toObject());
    m_actionTakenByAccount = account->adminIdentityLookupWithVanillaPointer(actionTakenByAccountdoc["id"_L1].toString(), actionTakenByAccountdoc.toObject());

    // remove this
    m_assignedModerator = !m_assignedAccount->userLevelIdentity()->account().isEmpty();
    // creating status array with the Post class
    const auto reportStatuses = doc[QStringLiteral("statuses")].toArray();
    std::transform(
        reportStatuses.cbegin(),
        reportStatuses.cend(),
        std::back_inserter(m_reportStatus),
        [ this, account ](const QJsonValue &value) -> auto{ return new Post(account, value.toObject(), this); });

    m_rules = doc["rules"_L1].toArray();

    Q_EMIT reportInfoUpdated();
}

#include "moc_reportinfo.cpp"