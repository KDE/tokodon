// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>
#include <QObject>

class AbstractAccount;
class FederationInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY federationInfoUpdated)
    Q_PROPERTY(QString domain READ domain NOTIFY federationInfoUpdated)
    Q_PROPERTY(QDateTime createdAt READ createdAt NOTIFY federationInfoUpdated)
    Q_PROPERTY(QString severity READ severity WRITE setSeverity NOTIFY severityUpdated)
    Q_PROPERTY(bool rejectMedia READ rejectMedia WRITE setRejectMedia NOTIFY rejectMediaUpdated)
    Q_PROPERTY(bool rejectReports READ rejectReports WRITE setRejectReports NOTIFY rejectReportsUpdated)
    Q_PROPERTY(QString privateComment READ privateComment WRITE setPrivateComment NOTIFY privateCommentUpdated)
    Q_PROPERTY(QString publicComment READ publicComment WRITE setPublicComment NOTIFY publicCommentUpdated)
    Q_PROPERTY(bool obfuscate READ obfuscate WRITE setObfuscate NOTIFY obfuscateUpdated)

public:
    QString id();
    QString domain();
    QDateTime createdAt();
    QString severity();
    void setSeverity(QString severity);
    bool rejectMedia();
    void setRejectMedia(bool rejectMedia);
    bool rejectReports();
    void setRejectReports(bool rejectReports);
    QString privateComment();
    void setPrivateComment(QString privateComment);
    QString publicComment();
    void setPublicComment(QString publicComment);
    bool obfuscate();
    void setObfuscate(bool obfuscate);

    void fromSourceData(const QJsonObject &doc);
    void reparentFederation(AbstractAccount *parent);

Q_SIGNALS:
    void severityUpdated();
    void rejectMediaUpdated();
    void rejectReportsUpdated();
    void privateCommentUpdated();
    void publicCommentUpdated();
    void obfuscateUpdated();
    void federationInfoUpdated();

private:
    QString m_id;
    QString m_domain;
    QString m_severity;
    QString m_privateComment;
    QString m_publicComment;
    QDateTime m_createdAt;
    bool m_rejectMedia = false;
    bool m_rejectReports = false;
    bool m_obfuscate = false;
    AbstractAccount *m_parent = nullptr;
};
