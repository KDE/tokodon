// SPDX-FileCopyrightText: 2022 Jeremy Winter <jeremy.winter@tutanota.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "relationship.h"
#include "account.h"

Relationship::Relationship(Identity *parent, const QJsonObject &jsonObj)
    : QObject(parent)
    , m_parent(parent)
{
    updateFromJson(jsonObj);
}

void Relationship::updateFromJson(const QJsonObject &jsonObj)
{
    setFollowing(jsonObj["following"].toBool());
    setRequested(jsonObj["requested"].toBool());
    setEndorsed(jsonObj["endorsed"].toBool());
    setFollowedBy(jsonObj["followed_by"].toBool());
    setMuting(jsonObj["muting"].toBool());
    setMutingNotifications(jsonObj["muting_notifications"].toBool());
    setShowingReblogs(jsonObj["showing_reblogs"].toBool());
    setNotifying(jsonObj["notifying"].toBool());
    setBlocking(jsonObj["blocking"].toBool());
    setDomainBlocking(jsonObj["domain_blocking"].toBool());
    setBlockedBy(jsonObj["blocked_by"].toBool());
    setNote(jsonObj["note"].toString());
}

bool Relationship::following() const
{
    return m_following;
}

void Relationship::setFollowing(bool following)
{
    if (following == m_following) {
        return;
    }
    m_following = following;
    Q_EMIT followingChanged();
}

bool Relationship::requested() const
{
    return m_requested;
}

void Relationship::setRequested(bool requested)
{
    if (requested == m_requested) {
        return;
    }
    m_requested = requested;
    Q_EMIT requestedChanged();
}

bool Relationship::endorsed() const
{
    return m_endorsed;
}

void Relationship::setEndorsed(bool endorsed)
{
    if (endorsed == m_endorsed) {
        return;
    }
    m_endorsed = endorsed;
    Q_EMIT endorsedChanged();
}

bool Relationship::followedBy() const
{
    return m_followedBy;
}

void Relationship::setFollowedBy(bool followedBy)
{
    if (followedBy == m_followedBy) {
        return;
    }
    m_followedBy = followedBy;
    Q_EMIT followedByChanged();
}

bool Relationship::muting() const
{
    return m_muting;
}

void Relationship::setMuting(bool muting)
{
    if (muting == m_muting) {
        return;
    }
    m_muting = muting;
    Q_EMIT mutingChanged();
}

bool Relationship::mutingNotifications() const
{
    return m_mutingNotifications;
}

void Relationship::setMutingNotifications(bool mutingNotifications)
{
    if (mutingNotifications == m_mutingNotifications) {
        return;
    }
    m_mutingNotifications = mutingNotifications;
    Q_EMIT mutingNotificationsChanged();
}

bool Relationship::showingReblogs() const
{
    return m_showingReblogs;
}

void Relationship::setShowingReblogs(bool showingReblogs)
{
    if (showingReblogs == m_showingReblogs) {
        return;
    }
    m_showingReblogs = showingReblogs;
    Q_EMIT showingReblogsChanged();
}

bool Relationship::notifying() const
{
    return m_notifying;
}

void Relationship::setNotifying(bool notifying)
{
    if (notifying == m_notifying) {
        return;
    }
    m_notifying = notifying;
    Q_EMIT notifyingChanged();
}

bool Relationship::blocking() const
{
    return m_blocking;
}

void Relationship::setBlocking(bool blocking)
{
    if (blocking == m_blocking) {
        return;
    }
    m_blocking = blocking;
    Q_EMIT blockingChanged();
}

bool Relationship::domainBlocking() const
{
    return m_domainBlocking;
}

void Relationship::setDomainBlocking(bool domainBlocking)
{
    if (domainBlocking == m_domainBlocking) {
        return;
    }
    m_domainBlocking = domainBlocking;
    Q_EMIT domainBlockingChanged();
}

bool Relationship::blockedBy() const
{
    return m_blockedBy;
}

void Relationship::setBlockedBy(bool blockedBy)
{
    if (blockedBy == m_blockedBy) {
        return;
    }
    m_blockedBy = blockedBy;
    Q_EMIT blockedByChanged();
}

QString Relationship::note() const
{
    return m_note;
}

void Relationship::setNote(const QString &note)
{
    if (note == m_note) {
        return;
    }
    m_note = note;
    Q_EMIT noteChanged();
}
