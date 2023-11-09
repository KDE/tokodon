// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "emailblocktoolmodel.h"

#include "abstractaccount.h"
#include "accountmanager.h"

EmailBlockToolModel::EmailBlockToolModel(QObject *parent)
    : QAbstractListModel(parent)
{
    filltimeline();
}

QVariant EmailBlockToolModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &emailInfo = m_emailinfo[index.row()];

    switch (role) {
    case IdRole:
        return emailInfo.id();
    case DomainRole:
        return emailInfo.domain();
    case CreatedAtRole:
        return emailInfo.createdAt();
    case IpSignUpCount:
        return emailInfo.ipSignupCount();
    case AccountSignUpCount:
        return emailInfo.accountSignupCount();
    default:
        return {};
    }
}

bool EmailBlockToolModel::loading() const
{
    return m_loading;
}

void EmailBlockToolModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int EmailBlockToolModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_emailinfo.count();
}

QHash<int, QByteArray> EmailBlockToolModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {DomainRole, "domain"},
        {CreatedAtRole, "createdAt"},
        {AccountSignUpCount, "accountCount"},
        {IpSignUpCount, "ipCount"},
    };
}

void EmailBlockToolModel::newEmailBlock(const QString &domain)
{
    const QJsonObject obj{
        {QStringLiteral("domain"), domain},
    };

    const auto doc = QJsonDocument(obj);

    const auto account = AccountManager::instance().selectedAccount();

    const QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/email_domain_blocks"));

    account->post(url, doc, true, this, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto jsonObj = doc.object();
        const auto newEmailInfo = EmailInfo::fromSourceData(jsonObj);

        beginInsertRows({}, m_emailinfo.size(), m_emailinfo.size());
        m_emailinfo += newEmailInfo;
        endInsertRows();
    });
}

void EmailBlockToolModel::deleteEmailBlock(const int row)
{
    const auto account = AccountManager::instance().selectedAccount();
    const auto &emailinfo = m_emailinfo[row];
    const auto emailBlockId = emailinfo.id();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/admin/email_domain_blocks/%1").arg(emailBlockId)), true, this, [=](QNetworkReply *reply) {
        Q_UNUSED(reply);
        beginRemoveRows({}, row, row);
        m_emailinfo.removeAt(row);
        endRemoveRows();
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void EmailBlockToolModel::filltimeline()
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);
    QUrl url;
    if (m_next.isEmpty()) {
        url = account->apiUrl(QStringLiteral("/api/v1/admin/email_domain_blocks"));
    } else {
        url = m_next;
    }

    account->get(url, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto emailblocks = doc.array();

        if (!emailblocks.isEmpty()) {
            static QRegularExpression re(QStringLiteral("<(.*)>; rel=\"next\""));
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(QString::fromUtf8(next));
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }
            QList<EmailInfo> fetchedEmailblocks;

            std::transform(
                emailblocks.cbegin(),
                emailblocks.cend(),
                std::back_inserter(fetchedEmailblocks),
                [=](const QJsonValue &value) -> auto{ return EmailInfo::fromSourceData(value.toObject()); });
            beginInsertRows({}, m_emailinfo.size(), m_emailinfo.size() + fetchedEmailblocks.size() - 1);
            m_emailinfo += fetchedEmailblocks;
            endInsertRows();
        }
        setLoading(false);
    });
}

#include "moc_emailblocktoolmodel.cpp"
