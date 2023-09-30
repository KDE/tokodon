// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "admin/accounttoolmodel.h"

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "account/relationship.h"
#include <KLocalizedString>
#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>
#include <qstringliteral.h>

using namespace Qt::Literals::StringLiterals;

AccountsToolModel::AccountsToolModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
    fetchSelectedAccountPosition();
}

bool AccountsToolModel::loading() const
{
    return m_loading;
}

void AccountsToolModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

QUrlQuery AccountsToolModel::buildQuery() const
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("origin"), m_location);
    query.addQueryItem(QStringLiteral("status"), m_moderationStatus);
    query.addQueryItem(QStringLiteral("role_ids"), m_role);
    query.addQueryItem(QStringLiteral("username"), m_username);
    query.addQueryItem(QStringLiteral("display_name"), m_displayName);
    query.addQueryItem(QStringLiteral("email"), m_email);
    query.addQueryItem(QStringLiteral("ip"), m_ip);
    return query;
}

QVariant AccountsToolModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto identity = m_accounts[index.row()].get();
    switch (role) {
    case CustomRoles::IdentityRole:
        return QVariant::fromValue<AdminAccountInfo *>(identity);
    default:
        return {};
    }
}

int AccountsToolModel::rowCount(const QModelIndex &) const
{
    return m_accounts.count();
}

QHash<int, QByteArray> AccountsToolModel::roleNames() const
{
    return {
        {CustomRoles::IdentityRole, "identity"},
    };
}

QString AccountsToolModel::location() const
{
    return m_location;
}

void AccountsToolModel::setLocation(const QString &location)
{
    if (location == m_location) {
        return;
    }
    m_location = location;
    Q_EMIT locationChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

QString AccountsToolModel::moderationStatus() const
{
    return m_location;
}

void AccountsToolModel::setModerationStatus(const QString &moderationStatus)
{
    if (moderationStatus == m_moderationStatus) {
        return;
    }
    m_moderationStatus = moderationStatus;
    Q_EMIT moderationStatusChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

QString AccountsToolModel::role() const
{
    return m_role;
}

void AccountsToolModel::setRole(const QString &role)
{
    if (role == m_role) {
        return;
    }
    m_role = role;
    Q_EMIT roleChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

QString AccountsToolModel::username() const
{
    return m_username;
}

void AccountsToolModel::setUsername(const QString &username)
{
    if (username == m_username) {
        return;
    }
    m_username = username;
    Q_EMIT usernameChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

QString AccountsToolModel::displayName() const
{
    return m_displayName;
}

void AccountsToolModel::setDisplayName(const QString &displayName)
{
    if (displayName == m_displayName) {
        return;
    }
    m_displayName = displayName;
    Q_EMIT displayNameChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

QString AccountsToolModel::email() const
{
    return m_email;
}

void AccountsToolModel::setEmail(const QString &email)
{
    if (email == m_email) {
        return;
    }
    m_email = email;
    Q_EMIT emailChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

QString AccountsToolModel::ip() const
{
    return m_ip;
}

void AccountsToolModel::setIp(const QString &ip)
{
    if (ip == m_ip) {
        return;
    }
    m_ip = ip;
    Q_EMIT ipChanged();
    // resetting everything before populating
    m_pagination = false;
    clear();
    fillTimeline();
}

int AccountsToolModel::selectedAccountPosition() const
{
    return m_selectedAccountPosition;
}

void AccountsToolModel::clear()
{
    beginResetModel();
    m_accounts.clear();
    endResetModel();
    setLoading(false);
}

void AccountsToolModel::approveAccount(const int row)
{
    executeAdminAction(row, AdminAccountAction::ApproveAccount);
}

void AccountsToolModel::rejectAccount(const int row)
{
    executeAdminAction(row, AdminAccountAction::RejectAccount);
}

void AccountsToolModel::enableAccount(const int row)
{
    executeAdminAction(row, AdminAccountAction::EnableDisabledAccount);
}

void AccountsToolModel::unsilenceAccount(const int row)
{
    executeAdminAction(row, AdminAccountAction::UnsilenceAccount);
}

void AccountsToolModel::unsuspendAccount(const int row)
{
    executeAdminAction(row, AdminAccountAction::UnsuspendAccount);
}
void AccountsToolModel::unsensitiveAccount(const int row)
{
    executeAdminAction(row, AdminAccountAction::UnmarkSensitiveAccount);
}

void AccountsToolModel::actionAgainstAccount(const int row, const QString &type, const bool &emailWarning, const QString &note)
{
    executeAdminAction(row,
                       AdminAccountAction::ActionAgainstAccount,
                       {{QStringLiteral("type"), type}, {QStringLiteral("send_email_notification"), emailWarning}, {QStringLiteral("text"), note}});
}

bool AccountsToolModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_next.isEmpty() && m_pagination;
}

void AccountsToolModel::deleteAccountData(const int row)
{
    auto account = AccountManager::instance().selectedAccount();
    auto identity = m_accounts[row];
    const auto accountId = identity->userLevelIdentity()->id();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/admin/accounts/%1").arg(accountId)), true, this, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll()).object();
        qDebug() << "DELETED: " << doc;
    });
}

void AccountsToolModel::executeAdminAction(const int row, AdminAccountAction adminAccountAction, const QJsonObject &extraArguments)
{
    auto identity = m_accounts[row];
    const QHash<AdminAccountAction, QString> accountActionMap = {
        {AdminAccountAction::ApproveAccount, QStringLiteral("/approve")},
        {AdminAccountAction::RejectAccount, QStringLiteral("/reject")},
        {AdminAccountAction::ActionAgainstAccount, QStringLiteral("/action")},
        {AdminAccountAction::EnableDisabledAccount, QStringLiteral("/enable")},
        {AdminAccountAction::UnsilenceAccount, QStringLiteral("/unsilence")},
        {AdminAccountAction::UnsuspendAccount, QStringLiteral("/unsuspend")},
        {AdminAccountAction::UnmarkSensitiveAccount, QStringLiteral("/unsensitive")},
    };

    const auto apiCall = accountActionMap[adminAccountAction];

    const auto accountId = identity->userLevelIdentity()->id();

    const QString accountApiUrl = QStringLiteral("/api/v1/admin/accounts/") + accountId + apiCall;

    const QJsonDocument doc(extraArguments);
    // to be used when receiving parameter from actionAgainstAccount
    const auto type = doc["type"_L1].toString();

    auto account = AccountManager::instance().selectedAccount();
    QUrl url = account->apiUrl(accountApiUrl);

    account->post(url, doc, true, this, [=](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();

        // Check if the request failed due to one account blocking the other
        if (!jsonObj.value("error"_L1).isUndefined()) {
            const QHash<AdminAccountAction, QString> accountActionMap = {
                {AdminAccountAction::ApproveAccount, i18n("Could not accept account")},
                {AdminAccountAction::RejectAccount, i18n("Could not reject account")},
                {AdminAccountAction::ActionAgainstAccount, i18n("Could not take action against the account")},
                {AdminAccountAction::EnableDisabledAccount, i18n("Could not enable  the disabled account")},
                {AdminAccountAction::UnsilenceAccount, i18n("Could not unsilence the account")},
                {AdminAccountAction::UnsuspendAccount, i18n("Could not unsuspend the account")},
                {AdminAccountAction::UnmarkSensitiveAccount, i18n("Could not mark the account as not sensitive")},
            };
            const auto errorMessage = accountActionMap[adminAccountAction];
            Q_EMIT account->errorOccured(errorMessage);
            return;
        }

        switch (adminAccountAction) {
        case AdminAccountAction::ApproveAccount:
            identity->setApproved(true);
            break;
        case AdminAccountAction::RejectAccount:
            identity->setApproved(false);
            break;
        case AdminAccountAction::ActionAgainstAccount:
            if (type == QStringLiteral("disable")) {
                identity->setDisabled(true);
            } else if (type == QStringLiteral("sensitive")) {
                identity->setSensitized(true);
            } else if (type == QStringLiteral("silence")) {
                identity->setSilence(true);
            } else if (type == QStringLiteral("suspend")) {
                identity->setSuspended(true);
            }
            break;
        case AdminAccountAction::EnableDisabledAccount:
            identity->setDisabled(false);
            break;
        case AdminAccountAction::UnsilenceAccount:
            identity->setSilence(false);
            break;
        case AdminAccountAction::UnsuspendAccount:
            identity->setSuspended(false);
            break;
        case AdminAccountAction::UnmarkSensitiveAccount:
            identity->setSensitized(false);
            break;
        }
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void AccountsToolModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    fillTimeline();
}

void AccountsToolModel::fetchSelectedAccountPosition()
{
    auto account = AccountManager::instance().selectedAccount();

    const auto id = account->identity()->id();

    QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/accounts/%1").arg(id));

    account->get(url, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        m_selectedAccountPosition = doc["role"_L1]["position"_L1].toInt();
    });
}

void AccountsToolModel::fillTimeline()
{
    // selecting the current logged in account
    auto account = AccountManager::instance().selectedAccount();

    // loading logic here
    m_pagination = true;
    if (m_loading) {
        return;
    }
    setLoading(true);

    QUrl url;
    if (m_next.isEmpty()) {
        url = account->apiUrl(QStringLiteral("/api/v2/admin/accounts"));
    } else {
        url = m_next;
    }
    // To be removed when the pagination api response in fixed
    if (url.toString().contains("v1"_L1)) {
        url = QUrl(url.toString().replace("/v1/"_L1, "/v2/"_L1));
    }

    url.setQuery(buildQuery());

    account->get(url, true, this, [this, account](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto accounts = doc.array();

        if (!accounts.isEmpty()) {
            static QRegularExpression re(QStringLiteral("<(.*)>; rel=\"next\""));
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(QString::fromUtf8(next));
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }
            QList<std::shared_ptr<AdminAccountInfo>> fetchedAccounts;

            std::transform(
                accounts.cbegin(),
                accounts.cend(),
                std::back_inserter(fetchedAccounts),
                [account](const QJsonValue &value) -> auto{
                    const auto identityJson = value.toObject();
                    return account->adminIdentityLookup(identityJson["id"_L1].toString(), identityJson);
                });
            beginInsertRows({}, m_accounts.size(), m_accounts.size() + fetchedAccounts.size() - 1);
            m_accounts += fetchedAccounts;
            endInsertRows();
        }

        setLoading(false);
    });
}
