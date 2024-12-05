// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

#include "admin/federationinfo.h"

class FederationToolModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

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

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);
    [[nodiscard]] FederationToolModel::FederationAction federationAction() const;
    void setFederationAction(const FederationToolModel::FederationAction &federationAction);
    void filltimeline(FederationAction action = FederationAction::BlockedDomains);

    Q_INVOKABLE void removeDomainBlock(int row);
    Q_INVOKABLE void removeAllowedDomain(int row);
    Q_INVOKABLE void updateDomainBlock(int row,
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
    QList<FederationInfo> m_federations;
    bool m_loading = false;
    FederationToolModel::FederationAction m_federationAction = FederationAction::BlockedDomains;
    std::optional<QUrl> m_next;
};
