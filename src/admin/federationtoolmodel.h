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
    Q_PROPERTY(QString moderation READ moderation WRITE setModeration NOTIFY moderationChanged)

public:
    enum CustomRoles {
        FederationRole = Qt::UserRole + 1,
    };

    enum FederationAction {
        AllowedDomains,
        BlockedDomains,
    };

    explicit FederationToolModel(QObject *parent = nullptr);

    void clear();

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;
    void setLoading(bool loading);
    QString moderation() const;
    void setModeration(QString moderation);

    Q_INVOKABLE void removeDomainBlock(const int row);
    Q_INVOKABLE void updateDomainBlock(const int row,
                                       QString severity,
                                       QString publicComment,
                                       QString privateComment,
                                       bool rejectMedia,
                                       bool rejectReports,
                                       bool obfuscateReport);
    Q_INVOKABLE void
    newDomainBlock(QString domain, QString severity, QString publicComment, QString privateComment, bool rejectMedia, bool rejectReports, bool obfuscateReport);

Q_SIGNALS:
    void loadingChanged();
    void moderationChanged();

protected:
private:
    void filltimeline(FederationAction action = FederationAction::BlockedDomains);

    QList<FederationInfo *> m_federations;
    bool m_loading = false;
    QString m_moderation = "Limited";
    QUrl m_next;
};
