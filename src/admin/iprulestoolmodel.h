// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

#include "admin/ipinfo.h"

class IpRulesToolModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles {
        IdRole,
        IpRole,
        SeverityRole,
        CommentRole,
        CreatedAtRole,
        ExpiredAtRole,
    };

    enum TimeInterval {
        ThreeYears = std::chrono::years{3}.count(),
        OneYear = std::chrono::years{1}.count(),
        Sixmonths = std::chrono::months{6}.count(),
        Onemonth = std::chrono::months{1}.count(),
        Twoweeks = std::chrono::weeks{2}.count(),
        Oneday = std::chrono::days{1}.count(),
    };

    Q_ENUM(TimeInterval)

    explicit IpRulesToolModel(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void filltimeline();

    Q_INVOKABLE void newIpBlock(const QString &ip, int expiresIn, const QString &comment, const QString &severity);
    Q_INVOKABLE void deleteIpBlock(int row);
    Q_INVOKABLE void updateIpBlock(int row, const QString &ip, const QString &severity, const QString &comment, int expiresIn);

Q_SIGNALS:
    void loadingChanged();

private:
    QList<IpInfo> m_ipinfo;
    bool m_loading = false;
    std::optional<QUrl> m_next;
};
