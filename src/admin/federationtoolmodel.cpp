// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "admin/federationtoolmodel.h"
#include "account/abstractaccount.h"
#include "account/accountmanager.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

FederationToolModel::FederationToolModel(QObject *parent)
    : QAbstractListModel(parent)
{
    filltimeline();
}

QVariant FederationToolModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &federationInfo = m_federations[index.row()];
    switch (role) {
    case IdRole:
        return federationInfo.id();
    case DomainRole:
        return federationInfo.domain();
    case CreatedAtRole:
        return federationInfo.createdAt();
    case SeverityRole:
        return federationInfo.severity();
    case RejectMediaRole:
        return federationInfo.rejectMedia();
    case RejectReportsRole:
        return federationInfo.rejectReports();
    case PrivateCommentRole:
        return federationInfo.privateComment();
    case PublicCommentRole:
        return federationInfo.publicComment();
    case ObfuscateRole:
        return federationInfo.obfuscate();
    default:
        return {};
    }
}

int FederationToolModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_federations.count();
}

QHash<int, QByteArray> FederationToolModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {DomainRole, "domain"},
        {CreatedAtRole, "createdAt"},
        {SeverityRole, "severity"},
        {RejectMediaRole, "rejectMedia"},
        {RejectReportsRole, "rejectReports"},
        {PrivateCommentRole, "privateComment"},
        {PublicCommentRole, "publicComment"},
        {ObfuscateRole, "obfuscate"},
    };
}

bool FederationToolModel::loading() const
{
    return m_loading;
}

void FederationToolModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

FederationToolModel::FederationAction FederationToolModel::federationAction() const
{
    return m_federationAction;
}

void FederationToolModel::setFederationAction(const FederationAction &federationAction)
{
    if (m_federationAction == federationAction) {
        return;
    }
    m_federationAction = federationAction;
    Q_EMIT federationActionChanged();
    clear();
    filltimeline(m_federationAction);
}

void FederationToolModel::removeDomainBlock(const int row)
{
    auto account = AccountManager::instance().selectedAccount();
    const auto &federationInfo = m_federations[row];
    const auto federationId = federationInfo.id();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/admin/domain_blocks/%1").arg(federationId)), true, this, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll()).object();
        beginRemoveRows({}, row, row);
        m_federations.removeAt(row);
        endRemoveRows();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void FederationToolModel::removeAllowedDomain(const int row)
{
    auto account = AccountManager::instance().selectedAccount();
    const auto &federationInfo = m_federations[row];
    const auto federationId = federationInfo.id();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/admin/domain_allows/%1").arg(federationId)), true, this, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll()).object();
        beginRemoveRows({}, row, row);
        m_federations.removeAt(row);
        endRemoveRows();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void FederationToolModel::updateDomainBlock(const int row,
                                            const QString &severity,
                                            const QString &publicComment,
                                            const QString &privateComment,
                                            const bool &rejectMedia,
                                            const bool &rejectReports,
                                            const bool &obfuscateReport)
{
    QJsonObject obj{
        {QStringLiteral("severity"), severity},
        {QStringLiteral("public_comment"), publicComment},
        {QStringLiteral("private_comment"), privateComment},
        {QStringLiteral("reject_reports"), rejectReports},
        {QStringLiteral("reject_media"), rejectMedia},
        {QStringLiteral("obfuscate"), obfuscateReport},
    };

    const auto doc = QJsonDocument(obj);

    const auto account = AccountManager::instance().selectedAccount();
    auto &federationInfo = m_federations[row];
    const auto federationId = federationInfo.id();

    account->put(account->apiUrl(QStringLiteral("/api/v1/admin/domain_blocks/%1").arg(federationId)),
                 doc,
                 true,
                 this,
                 [=, &federationInfo](QNetworkReply *reply) {
                     const auto doc = QJsonDocument::fromJson(reply->readAll());
                     const auto jsonObj = doc.object();

                     if (!jsonObj.value("error"_L1).isUndefined()) {
                         account->errorOccured(i18n("Error occured when making a PUT request to update the domain block."));
                     }
                     federationInfo.setPublicComment(publicComment);
                     federationInfo.setPrivateComment(privateComment);
                     federationInfo.setRejectMedia(rejectMedia);
                     federationInfo.setRejectReports(rejectReports);
                     federationInfo.setObfuscate(obfuscateReport);
                     federationInfo.setSeverity(severity);
                     Q_EMIT dataChanged(index(row, 0), index(row, 0));
                 });
}

void FederationToolModel::newDomainBlock(const QString &domain,
                                         const QString &severity,
                                         const QString &publicComment,
                                         const QString &privateComment,
                                         const bool &rejectMedia,
                                         const bool &rejectReports,
                                         const bool &obfuscateReport)
{
    QJsonObject obj{
        {QStringLiteral("severity"), severity},
        {QStringLiteral("domain"), domain},
        {QStringLiteral("public_comment"), publicComment},
        {QStringLiteral("private_comment"), privateComment},
        {QStringLiteral("reject_reports"), rejectReports},
        {QStringLiteral("reject_media"), rejectMedia},
        {QStringLiteral("obfuscate"), obfuscateReport},
    };

    const auto doc = QJsonDocument(obj);

    const auto account = AccountManager::instance().selectedAccount();

    const QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/domain_blocks"));

    account->post(url, doc, true, this, [=](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();
        auto newFederation = FederationInfo::fromSourceData(jsonObj);

        beginInsertRows({}, m_federations.size(), m_federations.size());
        m_federations += newFederation;
        endInsertRows();
    });
}

void FederationToolModel::newDomainAllow(const QString &domain)
{
    QJsonObject obj{
        {QStringLiteral("domain"), domain},
    };

    const auto doc = QJsonDocument(obj);

    const auto account = AccountManager::instance().selectedAccount();

    const QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/domain_allows"));

    account->post(url, doc, true, this, [=](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();
        auto newFederation = FederationInfo::fromSourceData(jsonObj);

        beginInsertRows({}, m_federations.size(), m_federations.size());
        m_federations += newFederation;
        endInsertRows();
    });
}

void FederationToolModel::clear()
{
    beginResetModel();
    m_federations.clear();
    m_next.clear();
    endResetModel();
    setLoading(false);
}

void FederationToolModel::filltimeline(FederationAction action)
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);
    QUrl url;
    if (m_next.isEmpty()) {
        switch (action) {
        case FederationAction::AllowedDomains:
            url = account->apiUrl(QStringLiteral("/api/v1/admin/domain_allows"));
            break;
        case FederationAction::BlockedDomains:
            url = account->apiUrl(QStringLiteral("/api/v1/admin/domain_blocks"));
            break;
        }
    } else {
        url = m_next;
    }

    account->get(url, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto federations = doc.array();

        if (!federations.isEmpty()) {
            static QRegularExpression re(QStringLiteral("<(.*)>; rel=\"next\""));
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(QString::fromUtf8(next));
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }
            QList<FederationInfo> fetchedFederations;

            std::transform(
                federations.cbegin(),
                federations.cend(),
                std::back_inserter(fetchedFederations),
                [=](const QJsonValue &value) -> auto{ return FederationInfo::fromSourceData(value.toObject()); });
            beginInsertRows({}, m_federations.size(), m_federations.size() + fetchedFederations.size() - 1);
            m_federations += fetchedFederations;
            endInsertRows();
        }
        setLoading(false);
    });
}

#include "moc_federationtoolmodel.cpp"