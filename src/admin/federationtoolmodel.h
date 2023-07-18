// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "admin/federationinfo.h"

#include <QAbstractListModel>
#include <QUrl>

class FederationToolModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(FederationAction federationAction READ federationAction WRITE setFederationAction NOTIFY federationActionChanged)

public:
    enum CustomRoles {
        IdRole,
        DomainRole,
        CreatedAtRole,
        SeverityRole,
        RejectMediaRole,
        RejectReportsRole,
        PrivateCommentRole,
        PublicCommentRole,
        ObfuscateRole
    };

    enum FederationAction {
        AllowedDomains,
        BlockedDomains,
    };

    Q_ENUM(FederationAction)

    explicit FederationToolModel(QObject *parent = nullptr);

    void clear();

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;
    void setLoading(bool loading);
    FederationToolModel::FederationAction federationAction() const;
    void setFederationAction(const FederationToolModel::FederationAction &federationAction);

    Q_INVOKABLE void removeDomainBlock(const int &row);
    Q_INVOKABLE void removeAllowedDomain(const int &row);
    Q_INVOKABLE void updateDomainBlock(const int &row,
                                       const QString &severity,
                                       const QString &publicComment,
                                       const QString &privateComment,
                                       const bool &rejectMedia,
                                       const bool &rejectReports,
                                       const bool &obfuscateReport);
    Q_INVOKABLE void newDomainBlock(const QString &domain,
                                    const QString &severity,
                                    const QString &publicComment,
                                    const QString &privateComment,
                                    const bool &rejectMedia,
                                    const bool &rejectReports,
                                    const bool &obfuscateReport);
    Q_INVOKABLE void newDomainAllow(const QString &domain);

Q_SIGNALS:
    void loadingChanged();
    void federationActionChanged();

private:
    void filltimeline(FederationAction action = FederationAction::BlockedDomains);

    QList<FederationInfo> m_federations;
    bool m_loading = false;
    FederationToolModel::FederationAction m_federationAction = FederationAction::BlockedDomains;
    QUrl m_next;
};
