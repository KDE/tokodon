// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>
#include <QObject>

class FederationInfo
{
public:
    QString id() const;
    QString domain() const;
    QDateTime createdAt() const;
    QString severity() const;
    void setSeverity(QString severity);
    bool rejectMedia() const;
    void setRejectMedia(bool rejectMedia);
    bool rejectReports() const;
    void setRejectReports(bool rejectReports);
    QString privateComment() const;
    void setPrivateComment(QString privateComment);
    QString publicComment() const;
    void setPublicComment(QString publicComment);
    bool obfuscate() const;
    void setObfuscate(bool obfuscate);

    static FederationInfo fromSourceData(const QJsonObject &doc);

private:
    QString m_id;
    QString m_domain;
    QString m_severity = "silence";
    QString m_privateComment;
    QString m_publicComment;
    QDateTime m_createdAt;
    bool m_rejectMedia = false;
    bool m_rejectReports = false;
    bool m_obfuscate = false;
};
