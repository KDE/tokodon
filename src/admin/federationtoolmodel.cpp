// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/abstractaccount.h"

#include "account/accountmanager.h"
#include "admin/federationtoolmodel.h"

FederationToolModel::FederationToolModel(QObject *parent)
    : QAbstractListModel(parent)
{
    filltimeline();
}

QVariant FederationToolModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto federationInfo = m_federations[index.row()];
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

QString FederationToolModel::moderation() const
{
    return m_moderation;
}

void FederationToolModel::setModeration(QString moderation)
{
    if (m_moderation == moderation) {
        return;
    }
    m_moderation = moderation;
    Q_EMIT moderationChanged();
    clear();
    if (moderation == "All") {
        filltimeline(FederationAction::AllowedDomains);
    } else {
        filltimeline(FederationAction::BlockedDomains);
    }
}

void FederationToolModel::removeDomainBlock(const int row)
{
    auto account = AccountManager::instance().selectedAccount();
    auto federationInfo = m_federations[row];
    const auto federationId = federationInfo.id();

    account->deleteResource(account->apiUrl(QString("/api/v1/admin/domain_blocks/%1").arg(federationId)), true, this, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll()).object();
        qDebug() << "DELETED: " << doc;
        beginRemoveRows({}, row, row);
        m_federations.removeAt(row);
        endRemoveRows();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void FederationToolModel::updatePublicComment(const int row, const QString &publicComment)
{
    auto federationInfo = m_federations[row];
    federationInfo.setPublicComment(publicComment);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void FederationToolModel::updateDomainBlock(const int row,
                                            QString severity,
                                            QString publicComment,
                                            QString privateComment,
                                            bool rejectMedia,
                                            bool rejectReports,
                                            bool obfuscateReport)
{
    QJsonObject obj{{"severity", severity},
                    {"public_comment", publicComment},
                    {"private_comment", privateComment},
                    {"reject_reports", rejectReports},
                    {"reject_media", rejectMedia},
                    {"obfuscate", obfuscateReport}};

    auto doc = QJsonDocument(obj);

    auto account = AccountManager::instance().selectedAccount();
    auto federationInfo = m_federations[row];
    const auto federationId = federationInfo.id();

    account->put(account->apiUrl(QString("/api/v1/admin/domain_blocks/%1").arg(federationId)), doc, true, this, nullptr);

    // trying to change the data in the model here
    federationInfo.setPublicComment(publicComment);

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void FederationToolModel::newDomainBlock(QString domain,
                                         QString severity,
                                         QString publicComment,
                                         QString privateComment,
                                         bool rejectMedia,
                                         bool rejectReports,
                                         bool obfuscateReport)
{
    QJsonObject obj{{"severity", severity},
                    {"domain", domain},
                    {"public_comment", publicComment},
                    {"private_comment", privateComment},
                    {"reject_reports", rejectReports},
                    {"reject_media", rejectMedia},
                    {"obfuscate", obfuscateReport}};

    auto doc = QJsonDocument(obj);

    auto account = AccountManager::instance().selectedAccount();

    QUrl url = account->apiUrl("/api/v1/admin/domain_blocks");

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
    endResetModel();
    setLoading(false);
}

void FederationToolModel::filltimeline(FederationAction action)
{
    auto account = AccountManager::instance().selectedAccount();

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
            static QRegularExpression re("<(.*)>; rel=\"next\"");
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(next);
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }
            QList<FederationInfo> fetchedFederations;

            std::transform(federations.cbegin(), federations.cend(), std::back_inserter(fetchedFederations), [](const QJsonValue &value) -> auto {
                return FederationInfo::fromSourceData(value.toObject());
            });
            beginInsertRows({}, m_federations.size(), m_federations.size() + fetchedFederations.size() - 1);
            m_federations += fetchedFederations;
            endInsertRows();
        }
        setLoading(false);
    });
}
