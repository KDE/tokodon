// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "annualreport.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

using namespace Qt::StringLiterals;

AnnualReport::AnnualReport() = default;

AnnualReport::AnnualReport(const QJsonObject &source)
    : m_year(source["year"_L1].toInt())
{
    const auto data = source["data"_L1].toObject();
    m_archetype = data["archetype"_L1].toString();
}

int AnnualReport::year() const
{
    return m_year;
}

QString AnnualReport::archetype() const
{
    return m_archetype;
}

AnnualReports::AnnualReports(QObject *parent)
    : QObject(parent)
{
}

AbstractAccount *AnnualReports::account() const
{
    return m_account;
}

void AnnualReports::setAccount(AbstractAccount *account)
{
    if (m_account == account) {
        return;
    }
    m_account = account;
    Q_EMIT accountChanged();

    if (!m_account) {
        return;
    }
    m_account->get(m_account->apiUrl(u"/api/v1/annual_reports"_s), true, this, [this](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        m_annualReports.clear();

        const auto reports = doc["annual_reports"_L1].toArray();
        std::ranges::transform(reports, std::back_inserter(m_annualReports), [=](const QJsonValue &value) -> AnnualReport {
            return AnnualReport{value.toObject()};
        });
        Q_EMIT annualReportsChanged();
    });
}

QList<AnnualReport> AnnualReports::annualReports() const
{
    return m_annualReports;
}
