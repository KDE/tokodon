// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "admin/federationinfo.h"

#include <QDateTime>
#include <QJsonObject>

QString FederationInfo::id()
{
    return m_id;
}

QString FederationInfo::domain()
{
    return m_domain;
}

QDateTime FederationInfo::createdAt()
{
    return m_createdAt;
}

QString FederationInfo::severity()
{
    return m_severity;
}

void FederationInfo::setSeverity(QString severity)
{
    if (m_severity == severity) {
        return;
    }
    m_severity = severity;
    Q_EMIT severityUpdated();
}

bool FederationInfo::rejectMedia()
{
    return m_rejectMedia;
}

void FederationInfo::setRejectMedia(bool rejectMedia)
{
    if (m_rejectMedia == rejectMedia) {
        return;
    }
    m_rejectMedia = rejectMedia;
    Q_EMIT rejectMediaUpdated();
}

bool FederationInfo::rejectReports()
{
    return m_rejectReports;
}

void FederationInfo::setRejectReports(bool rejectReports)
{
    if (m_rejectReports == rejectReports) {
        return;
    }
    m_rejectReports = rejectReports;
    Q_EMIT rejectReportsUpdated();
}

QString FederationInfo::privateComment()
{
    return m_privateComment;
}

void FederationInfo::setPrivateComment(QString privateComment)
{
    if (m_privateComment == privateComment) {
        return;
    }
    m_privateComment = privateComment;
    Q_EMIT privateCommentUpdated();
}

QString FederationInfo::publicComment()
{
    return m_publicComment;
}

void FederationInfo::setPublicComment(QString publicComment)
{
    if (m_publicComment == publicComment) {
        return;
    }
    m_publicComment = publicComment;
    Q_EMIT publicCommentUpdated();
}

bool FederationInfo::obfuscate()
{
    return m_obfuscate;
}

void FederationInfo::setObfuscate(bool obfuscate)
{
    if (m_obfuscate == obfuscate) {
        return;
    }
    m_obfuscate = obfuscate;
    Q_EMIT obfuscateUpdated();
}

void FederationInfo::fromSourceData(const QJsonObject &doc)
{
    m_id = doc["id"].toString();
    m_domain = doc["domain"].toString();
    m_createdAt = QDateTime::fromString(doc["created_at"].toString(), Qt::ISODate).toLocalTime();
    m_severity = doc["severity"].toString();
    m_rejectMedia = doc["reject_media"].toBool();
    m_rejectReports = doc["reject_reports"].toBool();
    m_privateComment = doc["private_comment"].toString();
    m_publicComment = doc["public_comment"].toString();
    m_obfuscate = doc["obfuscate"].toBool();

    Q_EMIT federationInfoUpdated();
}

void FederationInfo::reparentFederation(AbstractAccount *parent)
{
    m_parent = parent;
}
