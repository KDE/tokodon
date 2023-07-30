// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "admin/iprulestoolmodel.h"
#include "account/abstractaccount.h"
#include "account/accountmanager.h"

#include <KLocalizedString>

IpRulesToolModel::IpRulesToolModel(QObject *parent)
    : QAbstractListModel(parent)
{
    filltimeline();
}

QVariant IpRulesToolModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &ipInfo = m_ipinfo[index.row()];

    switch (role) {
    case IdRole:
        return ipInfo.id();
    case IpRole:
        return ipInfo.ip();
    case SeverityRole:
        return ipInfo.severity();
    case CommentRole:
        return ipInfo.comment();
    case CreatedAtRole:
        return ipInfo.createdAt();
    case ExpiredAtRole:
        return ipInfo.expiresAt();
    default:
        return {};
    }
}

bool IpRulesToolModel::loading() const
{
    return m_loading;
}

void IpRulesToolModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int IpRulesToolModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_ipinfo.count();
}

QHash<int, QByteArray> IpRulesToolModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {IpRole, "ip"},
        {SeverityRole, "severity"},
        {CommentRole, "comment"},
        {CreatedAtRole, "createdAt"},
        {ExpiredAtRole, "expiredAt"},
    };
}

void IpRulesToolModel::newIpBlock(const QString &ip, const int expiresIn, const QString &comment, const QString &severity)
{
    const QJsonObject obj{
        {"ip", ip},
        {"severity", severity},
        {"comment", comment},
        {"expires_in", expiresIn},
    };

    const auto doc = QJsonDocument(obj);

    const auto account = AccountManager::instance().selectedAccount();

    const QUrl url = account->apiUrl("/api/v1/admin/ip_blocks");

    account->post(url, doc, true, this, [=](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();
        auto newIpInfo = IpInfo::fromSourceData(jsonObj);

        beginInsertRows({}, m_ipinfo.size(), m_ipinfo.size());
        m_ipinfo += newIpInfo;
        endInsertRows();
    });
}

void IpRulesToolModel::updateIpBlock(const int row, const QString &ip, const QString &severity, const QString &comment, const int expiresAt)
{
    const QJsonObject obj{
        {"ip", ip},
        {"severity", severity},
        {"comment", comment},
        {"expires_in", expiresAt},
    };

    const auto doc = QJsonDocument(obj);

    const auto account = AccountManager::instance().selectedAccount();
    auto &ipInfo = m_ipinfo[row];
    const auto ipBlockId = ipInfo.id();

    account->put(account->apiUrl(QString("/api/v1/admin/ip_blocks/%1").arg(ipBlockId)), doc, true, this, [=, &ipInfo](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto jsonObj = doc.object();

        if (!jsonObj.value("error").isUndefined()) {
            account->errorOccured(i18n("Error occured when making a PUT request to update the domain block."));
        }
        ipInfo.setIp(ip);
        ipInfo.setSeverity(severity);
        ipInfo.setComment(comment);
        ipInfo.setExpiredAt(expiresAt);
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void IpRulesToolModel::deleteIpBlock(const int row)
{
    const auto account = AccountManager::instance().selectedAccount();
    const auto &ipInfo = m_ipinfo[row];
    const auto ipBlockId = ipInfo.id();

    account->deleteResource(account->apiUrl(QString("/api/v1/admin/ip_blocks/%1").arg(ipBlockId)), true, this, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll()).object();
        beginRemoveRows({}, row, row);
        m_ipinfo.removeAt(row);
        endRemoveRows();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void IpRulesToolModel::filltimeline()
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);
    QUrl url;
    if (m_next.isEmpty()) {
        url = account->apiUrl(QStringLiteral("/api/v1/admin/ip_blocks"));
    } else {
        url = m_next;
    }

    account->get(url, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto ipblocks = doc.array();

        if (!ipblocks.isEmpty()) {
            static QRegularExpression re("<(.*)>; rel=\"next\"");
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(next);
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }
            QList<IpInfo> fetchedIpblocks;

            std::transform(
                ipblocks.cbegin(),
                ipblocks.cend(),
                std::back_inserter(fetchedIpblocks),
                [=](const QJsonValue &value) -> auto{ return IpInfo::fromSourceData(value.toObject()); });
            beginInsertRows({}, m_ipinfo.size(), m_ipinfo.size() + fetchedIpblocks.size() - 1);
            m_ipinfo += fetchedIpblocks;
            endInsertRows();
        }
        setLoading(false);
    });
}
