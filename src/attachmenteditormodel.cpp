// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "attachmenteditormodel.h"
#include "account.h"
#include "abstractaccount.h"
#include <QTimer>

AttachmentEditorModel::AttachmentEditorModel(QObject *parent, AbstractAccount *account)
    : QAbstractListModel(parent)
	, m_account(account)
{
}

int AttachmentEditorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_attachments.size();
}

int AttachmentEditorModel::count() const
{
    return rowCount({});
}

QVariant AttachmentEditorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const int row = index.row();
    const auto &attachment = m_attachments[row];

    switch (role) {
    case Preview:
        return attachment.m_preview_url;
    case Description:
        return attachment.description();
    }

    return {};
}

bool AttachmentEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int row = index.row();

    if (role != Qt::EditRole) {
        return false;
    }

    if (row != Description) {
        return false;
    }

	const auto description = value.toString();
	auto &attachment = m_attachments[row];
	const auto id = attachment.id();
	attachment.setDescription(description);
	if (m_updateTimers.contains(id)) {
		auto timer = m_updateTimers[id];
		timer->stop();
		timer->deleteLater();
		m_updateTimers.remove(id);
	}

	auto timer = new QTimer(this);
	timer->setSingleShot(true);
	timer->setInterval(1000);
	connect(timer, &QTimer::timeout, this, [timer, id, row, this, description]() {
		timer->deleteLater();
		m_updateTimers.remove(id);
		const auto attachementUrl = m_account->apiUrl(QStringLiteral("/api/v1/media/%1").arg(id));
		const QJsonObject obj{
			{"description", description},
		};
		const QJsonDocument doc(obj);
		m_account->put(attachementUrl, doc, true, this, [row, description, this](QNetworkReply *reply) {
			auto &attachment = m_attachments[row];
		});
	});

    return true;
}

QHash<int, QByteArray> AttachmentEditorModel::roleNames() const
{
    return {
        {Preview, QByteArrayLiteral("preview")},
        {Description, QByteArrayLiteral("description")},
        {Qt::DisplayRole, QByteArrayLiteral("display")},
    };
}

QNetworkReply *AttachmentEditorModel::append(const QUrl &filename)
{
	if (rowCount({}) >= 4) {
		return nullptr;
	}
    return m_account->upload(filename, [=](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());

        if (!doc.isObject()) {
            return;
        }

		beginInsertRows({}, m_attachments.count(), m_attachments.count());
        m_attachments.append(Attachment{doc.object()});
		endInsertRows();
    });
}
