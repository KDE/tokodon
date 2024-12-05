// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QJsonArray>
#include <QQmlEngine>

class EmailInfo
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Only for enums")

public:
    enum SeverityValues { LimitSignUps, BlockSignUps, BlockAccess };

    Q_ENUM(SeverityValues)

    [[nodiscard]] QString id() const;
    [[nodiscard]] QString domain() const;
    [[nodiscard]] QDateTime createdAt() const;
    [[nodiscard]] int accountSignupCount() const;
    [[nodiscard]] int ipSignupCount() const;

    void calculateCount(const QJsonArray &history, EmailInfo &info);

    static EmailInfo fromSourceData(const QJsonObject &doc);

private:
    int m_accountSignupCount = 0;
    int m_ipSignupCount = 0;
    QString m_id;
    QString m_domain;
    QDateTime m_createdAt;
    QJsonArray m_history;
};
