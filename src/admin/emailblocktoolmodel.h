// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "admin/emailinfo.h"

#include <QAbstractListModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class EmailBlockToolModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles {
        IdRole,
        DomainRole,
        CreatedAtRole,
        AccountSignUpCount,
        IpSignUpCount,
    };

    explicit EmailBlockToolModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    void filltimeline();

    Q_INVOKABLE void newEmailBlock(const QString &domain);
    Q_INVOKABLE void deleteEmailBlock(int row);

Q_SIGNALS:
    void loadingChanged();

private:
    QList<EmailInfo> m_emailinfo;
    bool m_loading = false;
    QUrl m_next;
};
