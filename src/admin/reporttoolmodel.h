// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "reportinfo.h"

#include <QtQml>

class AdminAccountInfo;

class ReportToolModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString moderationStatus READ moderationStatus WRITE setModerationStatus NOTIFY moderationStatusChanged)
    Q_PROPERTY(QString origin READ origin WRITE setOrigin NOTIFY originChanged)

public:
    enum CustomRoles {
        ReportRole = Qt::UserRole + 1,
    };

    enum ReportAction {
        ResolveReport,
        UnresolveReport,
        AssignReport,
        UnassignReport,
    };

    explicit ReportToolModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;
    void setLoading(bool loading);

    QUrlQuery buildQuery() const;

    QString moderationStatus() const;
    void setModerationStatus(const QString &moderationStatus);

    QString origin() const;
    void setOrigin(const QString &origin);

    void clear();
    void fetchSelectedAccountDetails();

    Q_INVOKABLE void resolveReport(const int row);
    Q_INVOKABLE void unresolveReport(const int row);
    Q_INVOKABLE void assignReport(const int row);
    Q_INVOKABLE void unassignReport(const int row);
    Q_INVOKABLE void updateReport(const int row, const QString &type, const QList<int> &ruleIds);

Q_SIGNALS:
    void loadingChanged();
    void moderationStatusChanged();
    void originChanged();

protected:
    void fetchSelectedAccountPosition();
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void executeReportAction(const int row, ReportAction accountAction, const QJsonObject &extraArguments = {});

private:
    void fillTimeline();
    bool m_loading = false;
    QString m_accountId;
    QString m_moderationStatus;
    QString m_targetAccountId;
    QList<std::shared_ptr<ReportInfo>> m_reports;
    std::shared_ptr<AdminAccountInfo> m_selectedAccount;
    QString m_origin;
    bool m_pagination = true;
    QUrl m_next;
};
