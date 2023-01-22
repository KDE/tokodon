// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "posteditorbackend.h"
#include "abstractaccount.h"
#include "accountmanager.h"
#include "attachmenteditormodel.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>

PostEditorBackend::PostEditorBackend(QObject *parent)
    : QObject(parent)
    , m_idenpotencyKey(QUuid::createUuid().toString())
	, m_account(AccountManager::instance().selectedAccount())
    , m_attachmentEditorModel(new AttachmentEditorModel(this, m_account))
{
}

PostEditorBackend::~PostEditorBackend() = default;

QString PostEditorBackend::status() const
{
    return m_status;
}

void PostEditorBackend::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    Q_EMIT statusChanged();
}

QString PostEditorBackend::spoilerText() const
{
    return m_spoilerText;
}

void PostEditorBackend::setSpoilerText(const QString &spoilerText)
{
    if (m_spoilerText == spoilerText) {
        return;
    }
    m_spoilerText = spoilerText;
    Q_EMIT spoilerTextChanged();
}

QString PostEditorBackend::inReplyTo() const
{
    return m_inReplyTo;
}

void PostEditorBackend::setInReplyTo(const QString &inReplyTo)
{
    if (m_inReplyTo == inReplyTo) {
        return;
    }
    m_inReplyTo = inReplyTo;
    Q_EMIT inReplyToChanged();
}

Post::Visibility PostEditorBackend::visibility() const
{
    return m_visibility;
}

void PostEditorBackend::setVisibility(Post::Visibility visibility)
{
    if (m_visibility == visibility) {
        return;
    }
    m_visibility = visibility;
    Q_EMIT visibilityChanged();
}

QString PostEditorBackend::language() const
{
    return m_language;
}

void PostEditorBackend::setLanguage(const QString &language)
{
    if (m_language == language) {
        return;
    }
    m_language = language;
    Q_EMIT languageChanged();
}

QDateTime PostEditorBackend::scheduledAt() const
{
    return m_scheduledAt;
}

void PostEditorBackend::setScheduledAt(const QDateTime &scheduledAt)
{
    if (m_scheduledAt == scheduledAt) {
        return;
    }
    m_scheduledAt = scheduledAt;
    Q_EMIT scheduledAtChanged();
}

AttachmentEditorModel *PostEditorBackend::attachmentEditorModel() const
{
    return m_attachmentEditorModel;
}

QStringList PostEditorBackend::mentions() const
{
    return m_mentions;
}

void PostEditorBackend::setMentions(const QStringList &mentions)
{
    if (m_mentions == mentions) {
        return;
    }
    m_mentions = mentions;
    Q_EMIT mentionsChanged();
}

AbstractAccount *PostEditorBackend::account() const
{
	return m_account;
}

void PostEditorBackend::setAccount(AbstractAccount *account)
{
	if (m_account == account) {
		return;
	}
	m_account = account;
	Q_EMIT accountChanged();
}

QJsonDocument PostEditorBackend::toJsonDocument() const
{
    QJsonObject obj;

    obj["spoiler_text"] = m_spoilerText;
    obj["status"] = m_status;
    // obj["content_type"] = m_content_type;
    // obj["sensitive"] = m_sensitive;
    // obj["visibility"] = visibilityToString(m_visibility);

    if (!m_inReplyTo.isEmpty()) {
        obj["in_reply_to_id"] = m_inReplyTo;
    }

    auto media_ids = QJsonArray();
    for (const auto &att : qAsConst(m_attachmentEditorModel->attachments())) {
        media_ids.append(att.m_id);
    }

    obj["media_ids"] = media_ids;

    return QJsonDocument(obj);
}

void PostEditorBackend::save()
{
    QUrl post_status_url = m_account->apiUrl("/api/v1/statuses");
    auto doc = toJsonDocument();

    m_account->post(post_status_url, doc, true, this, [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        qDebug() << "Message sent:" << obj;
    });
}
