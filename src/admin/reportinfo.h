// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QJsonArray>
#include <QQmlListProperty>

class AbstractAccount;
class AdminAccountInfo;
class Post;

class ReportInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString reportId READ reportId NOTIFY reportInfoUpdated)
    Q_PROPERTY(bool actionTaken READ actionTaken WRITE setActionTaken NOTIFY actionTakenUpdated)
    Q_PROPERTY(QDateTime actionTakenAt READ actionTakenAt NOTIFY reportInfoUpdated)
    Q_PROPERTY(QString category READ category NOTIFY reportInfoUpdated)
    Q_PROPERTY(QString comment READ comment NOTIFY reportInfoUpdated)
    Q_PROPERTY(bool forwarded READ forwarded NOTIFY reportInfoUpdated)
    Q_PROPERTY(QDateTime createdAt READ createdAt NOTIFY reportInfoUpdated)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt NOTIFY reportInfoUpdated)
    Q_PROPERTY(int statusCount READ statusCount NOTIFY reportInfoUpdated)
    Q_PROPERTY(int mediaAttachmentCount READ mediaAttachmentCount NOTIFY reportInfoUpdated)
    Q_PROPERTY(AdminAccountInfo *filedAccount READ filedAccount NOTIFY reportInfoUpdated)
    Q_PROPERTY(AdminAccountInfo *targetAccount READ targetAccount NOTIFY reportInfoUpdated)
    Q_PROPERTY(bool assignedModerator READ assignedModerator WRITE setAssignedModerator NOTIFY assignedModeratorUpdated)
    Q_PROPERTY(AdminAccountInfo *assignedAccount READ assignedAccount WRITE setAssignedAccount NOTIFY assignedAccountUpdated)
    Q_PROPERTY(AdminAccountInfo *actionTakenByAccount READ actionTakenByAccount NOTIFY reportInfoUpdated)
    Q_PROPERTY(QQmlListProperty<Post> reportStatus READ reportStatusList NOTIFY reportInfoUpdated)
    Q_PROPERTY(QJsonArray rules READ rules NOTIFY reportInfoUpdated)

public:
    explicit ReportInfo(QObject *parent = nullptr);
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
    [[nodiscard]] QQmlListProperty<Post> reportStatusList() const;
    [[nodiscard]] QJsonArray rules() const;
    void fromSourceData(const QJsonObject &doc);
    void reparentReportInfo(AbstractAccount *parent);

Q_SIGNALS:
    void actionTakenUpdated();
    void assignedModeratorUpdated();
    void assignedAccountUpdated();
    void reportInfoUpdated();

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
    QQmlListProperty<Post> m_reportStatusList;
    QJsonArray m_rules;
    AbstractAccount *m_parent = nullptr;
};
