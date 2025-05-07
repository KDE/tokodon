// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QJsonArray>
#include <QQmlListProperty>

class AbstractAccount;
class AdminAccountInfo;
class Post;

class ReportInfo
{
    Q_GADGET
    Q_PROPERTY(QString reportId READ reportId CONSTANT)
    Q_PROPERTY(bool actionTaken READ actionTaken WRITE setActionTaken)
    Q_PROPERTY(QDateTime actionTakenAt READ actionTakenAt CONSTANT)
    Q_PROPERTY(QString category READ category CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(bool forwarded READ forwarded CONSTANT)
    Q_PROPERTY(QDateTime createdAt READ createdAt CONSTANT)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt CONSTANT)
    Q_PROPERTY(int statusCount READ statusCount CONSTANT)
    Q_PROPERTY(int mediaAttachmentCount READ mediaAttachmentCount CONSTANT)
    Q_PROPERTY(AdminAccountInfo *filedAccount READ filedAccount CONSTANT)
    Q_PROPERTY(AdminAccountInfo *targetAccount READ targetAccount CONSTANT)
    Q_PROPERTY(bool assignedModerator READ assignedModerator WRITE setAssignedModerator)
    Q_PROPERTY(AdminAccountInfo *assignedAccount READ assignedAccount WRITE setAssignedAccount CONSTANT)
    Q_PROPERTY(AdminAccountInfo *actionTakenByAccount READ actionTakenByAccount CONSTANT)
    Q_PROPERTY(QQmlListProperty<Post> reportStatus READ reportStatus CONSTANT)
    Q_PROPERTY(QJsonArray rules READ rules CONSTANT)

public:
    explicit ReportInfo();
    [[nodiscard]] QString reportId() const;
    [[nodiscard]] bool actionTaken() const;
    void setActionTaken(bool actionTaken);
    [[nodiscard]] bool assignedModerator() const;
    void setAssignedModerator(bool moderatorAssigned);
    [[nodiscard]] QDateTime actionTakenAt() const;
    [[nodiscard]] QString category() const;
    [[nodiscard]] QString comment() const;
    [[nodiscard]] bool forwarded() const;
    [[nodiscard]] QDateTime createdAt() const;
    [[nodiscard]] QDateTime updatedAt() const;
    [[nodiscard]] int statusCount() const;
    [[nodiscard]] int mediaAttachmentCount() const;
    [[nodiscard]] AdminAccountInfo *filedAccount() const;
    [[nodiscard]] AdminAccountInfo *targetAccount() const;
    [[nodiscard]] AdminAccountInfo *assignedAccount() const;
    void setAssignedAccount(AdminAccountInfo *newAssignedAccount);
    [[nodiscard]] AdminAccountInfo *actionTakenByAccount() const;
    [[nodiscard]] QList<Post *> reportStatus() const;
    [[nodiscard]] QJsonArray rules() const;
    void fromSourceData(const QJsonObject &doc);

private:
    QString m_reportId;
    bool m_actionTaken = false;
    QDateTime m_actionTakenAt;
    QString m_category;
    QString m_comment;
    bool m_forwarded = false;
    bool m_assignedModerator = false;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    AdminAccountInfo *m_filedAccount = nullptr;
    AdminAccountInfo *m_targetAccount = nullptr;
    AdminAccountInfo *m_assignedAccount = nullptr;
    AdminAccountInfo *m_actionTakenByAccount = nullptr;
    QList<Post *> m_reportStatus;
    QJsonArray m_rules;
};
