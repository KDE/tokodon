// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "abstractaccount.h"
#include <QObject>
#include <qqmlintegration.h>

struct AnnualReport {
    Q_GADGET
    QML_VALUE_TYPE(annualReport)

    Q_PROPERTY(int year READ year CONSTANT)
    Q_PROPERTY(QString archetype READ archetype CONSTANT)

public:
    AnnualReport();
    explicit AnnualReport(const QJsonObject &source);

    int year() const;
    QString archetype() const;

private:
    int m_year;
    QString m_archetype;
};

/**
 * @brief Account-specific preferences which are stored server-side.
 */
class AnnualReports : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)

    Q_PROPERTY(QList<AnnualReport> annualReports READ annualReports NOTIFY annualReportsChanged)

public:
    explicit AnnualReports(QObject *parent = nullptr);

    QList<AnnualReport> annualReports() const;

    AbstractAccount *account() const;
    void setAccount(AbstractAccount *account);

Q_SIGNALS:
    void accountChanged();
    void annualReportsChanged();

private:
    AbstractAccount *m_account = nullptr;
    QList<AnnualReport> m_annualReports;
};
