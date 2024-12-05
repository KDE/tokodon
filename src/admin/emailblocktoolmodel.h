// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

#include "admin/emailinfo.h"

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

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void filltimeline();

    Q_INVOKABLE void newEmailBlock(const QString &domain);
    Q_INVOKABLE void deleteEmailBlock(int row);

Q_SIGNALS:
    void loadingChanged();

private:
    QList<EmailInfo> m_emailinfo;
    bool m_loading = false;
    std::optional<QUrl> m_next;
};
