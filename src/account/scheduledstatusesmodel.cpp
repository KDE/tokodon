// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "account/scheduledstatusesmodel.h"

#include "networkcontroller.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>

#include "texthandler.h"

using namespace Qt::StringLiterals;

ScheduledStatusesModel::ScheduledStatusesModel(QObject *parent)
    : AbstractListModel(parent)
{
    fill();
}

ScheduledStatusesModel::~ScheduledStatusesModel() = default;

QVariant ScheduledStatusesModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &status = m_statuses[index.row()];

    switch (role) {
    case IdRole:
        return QVariant::fromValue(status.id);
    case ScheduledAtRole: {
        if (m_drafts) {
            return TextHandler::getRelativeDateTime(status.scheduledAt.addYears(-DRAFT_YEAR));
        }
        return status.scheduledAt.toString(Qt::DateFormat::TextDate);
    }
    case TextRole:
        return QVariant::fromValue(status.text);
    default:
        return {};
    }
}

int ScheduledStatusesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_statuses.size();
}

QHash<int, QByteArray> ScheduledStatusesModel::roleNames() const
{
    return {{IdRole, "id"}, {ScheduledAtRole, "scheduledAt"}, {TextRole, "text"}};
}

QString ScheduledStatusesModel::displayName() const
{
    return m_drafts ? i18nc("@title:window", "Draft Posts") : i18nc("@title:window", "Scheduled Posts");
}

bool ScheduledStatusesModel::drafts() const
{
    return m_drafts;
}

void ScheduledStatusesModel::setDrafts(bool draft)
{
    m_drafts = draft;
    Q_EMIT draftsChanged();
}

void ScheduledStatusesModel::fill()
{
    if (loading()) {
        return;
    }
    setLoading(true);

    account()->get(
        account()->apiUrl(QStringLiteral("/api/v1/scheduled_statuses")),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto statuses = doc.array().toVariantList();
            std::ranges::reverse(statuses);

            if (!statuses.isEmpty()) {
                QList<ScheduledStatus> fetchedStatuses;

                std::ranges::transform(std::as_const(statuses), std::back_inserter(fetchedStatuses), [this](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                const auto [first, last] = std::ranges::remove_if(fetchedStatuses, [this](const ScheduledStatus &status) {
                    const bool isDraft = status.scheduledAt.date().year() >= QDate::currentDate().year() + DRAFT_YEAR;
                    return m_drafts != isDraft;
                });
                fetchedStatuses.erase(first, last);
                beginInsertRows({}, m_statuses.size(), m_statuses.size() + fetchedStatuses.size() - 1);
                m_statuses += fetchedStatuses;
                endInsertRows();
            }

            setLoading(false);
        },
        [this](QNetworkReply *reply) {
            setLoading(false);
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

void ScheduledStatusesModel::deleteDraft(const QModelIndex index)
{
    auto status = m_statuses[index.row()];
    account()->deleteResource(account()->apiUrl(QStringLiteral("/api/v1/scheduled_statuses/%1").arg(status.id)),
                              true,
                              this,
                              [this, index](QNetworkReply *reply) {
                                  Q_UNUSED(reply)
                                  beginRemoveRows({}, index.row(), index.row());
                                  m_statuses.removeAt(index.row());
                                  endRemoveRows();
                              });
}

ScheduledStatusesModel::ScheduledStatus ScheduledStatusesModel::fromSourceData(const QJsonObject &object) const
{
    ScheduledStatus status;
    status.id = object["id"_L1].toString();
    status.scheduledAt = QDateTime::fromString(object["scheduled_at"_L1].toString(), Qt::ISODate).toLocalTime();
    status.text = object["params"_L1].toObject()["text"_L1].toString();

    return status;
}

#include "moc_scheduledstatusesmodel.cpp"
