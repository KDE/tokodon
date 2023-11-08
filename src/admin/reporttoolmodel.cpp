// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QStringLiteral>
#include <QUrlQuery>

#include <KLocalizedString>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "admin/reporttoolmodel.h"

using namespace Qt::StringLiterals;

ReportToolModel::ReportToolModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
    fetchSelectedAccountDetails();
}

bool ReportToolModel::loading() const
{
    return m_loading;
}

void ReportToolModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

QUrlQuery ReportToolModel::buildQuery() const
{
    QUrlQuery query;
    if (m_moderationStatus == QStringLiteral("resolved")) {
        query.addQueryItem(QStringLiteral("resolved"), QStringLiteral("true"));
    }
    if (!m_accountId.isEmpty()) {
        query.addQueryItem(QStringLiteral("account_id"), m_accountId);
    }
    if (!m_targetAccountId.isEmpty()) {
        query.addQueryItem(QStringLiteral("target_account_id"), m_targetAccountId);
    }
    return query;
}

QVariant ReportToolModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto identity = m_reports[index.row()].get();
    switch (role) {
    case CustomRoles::ReportRole:
        return QVariant::fromValue<ReportInfo *>(identity);
    default:
        return {};
    }
}

int ReportToolModel::rowCount(const QModelIndex &) const
{
    return m_reports.count();
}

QHash<int, QByteArray> ReportToolModel::roleNames() const
{
    return {
        {CustomRoles::ReportRole, "reportInfo"},
    };
}

QString ReportToolModel::moderationStatus() const
{
    return m_moderationStatus;
}

void ReportToolModel::setModerationStatus(const QString &moderationStatus)
{
    if (moderationStatus == m_moderationStatus) {
        return;
    }
    m_moderationStatus = moderationStatus;
    Q_EMIT moderationStatusChanged();
    m_pagination = false;
    clear();
    fillTimeline();
}

QString ReportToolModel::origin() const
{
    return m_origin;
}

void ReportToolModel::setOrigin(const QString &origin)
{
    if (origin == m_origin) {
        return;
    }
    m_origin = origin;
    Q_EMIT moderationStatusChanged();
    m_pagination = false;
    clear();
    fillTimeline();
}

void ReportToolModel::clear()
{
    beginResetModel();
    m_reports.clear();
    endResetModel();
    setLoading(false);
}

void ReportToolModel::resolveReport(const int row)
{
    executeReportAction(row, ReportAction::ResolveReport);
}

void ReportToolModel::unresolveReport(const int row)
{
    executeReportAction(row, ReportAction::UnresolveReport);
}

void ReportToolModel::assignReport(const int row)
{
    executeReportAction(row, ReportAction::AssignReport);
}

void ReportToolModel::unassignReport(const int row)
{
    executeReportAction(row, ReportAction::UnassignReport);
}

void ReportToolModel::updateReport(const int row, const QString &type, const QList<int> &ruleIds)
{
    QJsonArray ruleIdArray;
    for (const int ruleId : ruleIds) {
        ruleIdArray.append(ruleId);
    }
    QJsonObject obj{{"category"_L1, type}, {"rule_ids"_L1, ruleIdArray}};
    const auto doc = QJsonDocument(obj);
    const auto account = AccountManager::instance().selectedAccount();
    const auto report = m_reports[row];
    const auto reportId = report->reportId();
    QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/reports/%1").arg(reportId));
    account->put(url, doc, true, this, [=](QNetworkReply *) {});
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void ReportToolModel::fetchSelectedAccountDetails()
{
    auto account = AccountManager::instance().selectedAccount();

    const auto id = account->identity()->id();

    QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/accounts/%1").arg(id));

    account->get(url, true, this, [this, account, id](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        m_selectedAccount = account->adminIdentityLookup(id, doc.object());
    });
    Q_EMIT moderationStatusChanged();
}

void ReportToolModel::executeReportAction(const int row, ReportAction reportAction, const QJsonObject &extraArguments)
{
    auto report = m_reports[row];
    const QHash<ReportAction, QString> reportActionMap = {
        {ReportAction::ResolveReport, QStringLiteral("/resolve")},
        {ReportAction::UnresolveReport, QStringLiteral("/reopen")},
        {ReportAction::AssignReport, QStringLiteral("/assign_to_self")},
        {ReportAction::UnassignReport, QStringLiteral("/unassign")},
    };

    const auto apiCall = reportActionMap[reportAction];

    const auto reportId = report->reportId();

    const QString reportApiUrl = QStringLiteral("/api/v1/admin/reports/%1/%2").arg(reportId, apiCall);

    const QJsonDocument doc(extraArguments);

    auto account = AccountManager::instance().selectedAccount();
    QUrl url = account->apiUrl(reportApiUrl);

    account->post(url, doc, true, this, [=](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();

        if (!jsonObj.value("error"_L1).isUndefined()) {
            const QHash<ReportAction, QString> accountActionMap = {
                {ReportAction::ResolveReport, i18n("Could not resolve report")},
                {ReportAction::UnresolveReport, i18n("Could not unresolve report")},
                {ReportAction::AssignReport, i18n("Could not assign report")},
                {ReportAction::UnassignReport, i18n("Could not unassign report")},
            };
            const auto errorMessage = accountActionMap[reportAction];
            Q_EMIT account->errorOccured(errorMessage);
            return;
        }

        switch (reportAction) {
        case ReportAction::ResolveReport:
            report->setActionTaken(true);
            break;
        case ReportAction::UnresolveReport:
            report->setActionTaken(false);
            break;
        case ReportAction::AssignReport:
            report->setAssignedModerator(true);
            report->setAssignedAccount(m_selectedAccount.get());
            break;
        case ReportAction::UnassignReport:
            report->setAssignedModerator(false);
            report->setAssignedAccount({});
            break;
        }
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

bool ReportToolModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_next.isEmpty() && m_pagination;
}

void ReportToolModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    fillTimeline();
}

void ReportToolModel::fillTimeline()
{
    auto account = AccountManager::instance().selectedAccount();
    m_pagination = true;

    if (m_loading) {
        return;
    }
    setLoading(true);

    QUrl url;
    if (m_next.isEmpty()) {
        url = account->apiUrl(QStringLiteral("/api/v1/admin/reports"));
    } else {
        url = m_next;
    }
    url.setQuery(buildQuery());

    account->get(url, true, this, [this, account](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto reportsArray = doc.array();

        if (!reportsArray.isEmpty()) {
            static QRegularExpression re(QStringLiteral("<(.*)>; rel=\"next\""));
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(QString::fromUtf8(next));
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }
            QList<std::shared_ptr<ReportInfo>> fetchedReports;

            std::transform(
                reportsArray.cbegin(),
                reportsArray.cend(),
                std::back_inserter(fetchedReports),
                [ account, this ](const QJsonValue &value) -> auto{
                    const auto reportInfoJson = value.toObject();
                    const auto accountPopulate = account->reportInfoLookup(reportInfoJson["id"_L1].toString(), reportInfoJson);
                    // hack to determine the report's origin to be removed when we have the specific query for it
                    if (m_origin == QStringLiteral("local") && accountPopulate->targetAccount()->isLocal()) {
                        return accountPopulate;
                    } else if (m_origin == QStringLiteral("remote") && !accountPopulate->targetAccount()->isLocal()) {
                        return accountPopulate;
                    } else if (m_origin.isEmpty()) {
                        return accountPopulate;
                    }
                    return std::shared_ptr<ReportInfo>();
                });
            beginInsertRows({}, m_reports.size(), m_reports.size() + fetchedReports.size() - 1);
            m_reports += fetchedReports;
            endInsertRows();
        }
        setLoading(false);
    });
}

#include "moc_reporttoolmodel.cpp"