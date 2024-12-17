// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "editor/posteditorbackend.h"

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "editor/attachmenteditormodel.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

PostEditorBackend::PostEditorBackend(QObject *parent)
    : QObject(parent)
    , m_idenpotencyKey(QUuid::createUuid().toString())
    , m_language(AccountManager::instance().selectedAccount()->preferences()->defaultLanguage())
    , m_poll(new PollEditorBackend(this))
    , m_account(AccountManager::instance().selectedAccount())
    , m_attachmentEditorModel(new AttachmentEditorModel(this, m_account))
{
}

PostEditorBackend::~PostEditorBackend() = default;

QString PostEditorBackend::id() const
{
    return m_id;
}

void PostEditorBackend::setId(const QString &id)
{
    m_id = id;
}

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

bool PostEditorBackend::sensitive() const
{
    return m_sensitive;
}

void PostEditorBackend::setSensitive(bool sensitive)
{
    if (m_sensitive == sensitive) {
        return;
    }
    m_sensitive = sensitive;
    Q_EMIT sensitiveChanged();
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

void PostEditorBackend::setHasExistingPoll(bool hasExisting)
{
    if (m_pollEnabled != hasExisting) {
        m_pollEnabled = hasExisting;
        Q_EMIT pollEnabledChanged();
    }
}

int PostEditorBackend::charactersLeft() const
{
    if (m_account == nullptr) {
        return 0;
    }

    QRegularExpression re{QStringLiteral(R"((http|https):\/\/([\w_-]+(?:(?:\.[\w_-]+)+))([\w.,@?^=%&:\/~+#-]*[\w@?^=%&\/~+#-]))")};
    re.setPatternOptions(QRegularExpression::DontCaptureOption);

    const auto matches = re.match(m_status).capturedTexts();

    // We want to accumulate each link, and then "add back" the characters you should
    // have, taking the difference from charactersPreservedPerUrl.
    const int sum = std::accumulate(matches.constBegin(), matches.constEnd(), 0, [this](int sum, const QString &link) {
        return sum + link.length() - m_account->charactersReservedPerUrl();
    });

    return m_account->maxPostLength() - m_status.length() + sum;
}

void PostEditorBackend::copyFromOther(PostEditorBackend *other)
{
    setId(other->id());
    setSpoilerText(other->spoilerText());
    setVisibility(other->visibility());
    setLanguage(other->language());
    setScheduledAt(other->scheduledAt());
    setMentions(other->mentions());
    setSensitive(other->sensitive());
    setAccount(other->account());
    m_pollEnabled = other->m_pollEnabled;
    Q_EMIT pollEnabledChanged();
    m_poll->copyFromOther(other->m_poll);
    m_attachmentEditorModel->copyFromOther(other->m_attachmentEditorModel);
}

void PostEditorBackend::setupReplyTo(Post *post)
{
    setInReplyTo(post->originalPostId());
    setSpoilerText(post->spoilerText());
    setVisibility(post->visibility());
    QStringList mentions = {'@'_L1 + post->authorIdentity()->account()};
    for (const auto &mention : post->mentions()) {
        if (mentions.contains(mention)) {
            continue;
        }
        mentions.append(mention);
    }
    setMentions(mentions);
}

QJsonDocument PostEditorBackend::toJsonDocument() const
{
    QJsonObject obj;

    obj["spoiler_text"_L1] = m_spoilerText;
    obj["status"_L1] = m_status;
    obj["sensitive"_L1] = m_sensitive;
    obj["visibility"_L1] = Post::visibilityToString(m_visibility);

    if (!m_inReplyTo.isEmpty()) {
        obj["in_reply_to_id"_L1] = m_inReplyTo;
    }

    auto media_ids = QJsonArray();
    for (const auto &att : std::as_const(m_attachmentEditorModel->attachments())) {
        media_ids.append(att->m_id);
    }

    obj["media_ids"_L1] = media_ids;
    obj["language"_L1] = m_language;

    if (m_pollEnabled) {
        obj["poll"_L1] = m_poll->toJsonObject();
    }

    if (m_scheduledAt.isValid()) {
        obj["scheduled_at"_L1] = m_scheduledAt.toString(Qt::DateFormat::ISODate);
    }

    return QJsonDocument(obj);
}

void PostEditorBackend::save()
{
    // If we were editing a scheduled or draft post, make sure to clean up the previous iteration
    if (!m_scheduledPostId.isEmpty()) {
        m_account->deleteResource(m_account->apiUrl(QStringLiteral("/api/v1/scheduled_statuses/%1").arg(m_scheduledPostId)), true, this, {});
    }

    QUrl post_status_url = m_account->apiUrl(QStringLiteral("/api/v1/statuses"));
    auto doc = toJsonDocument();

    QHash<QByteArray, QByteArray> headers;
    headers["Idempotency-Key"] = m_idenpotencyKey.toUtf8();

    m_account->post(
        post_status_url,
        doc,
        true,
        this,
        [this](QNetworkReply *) {
            Q_EMIT posted(QStringLiteral(""));
        },
        [this](QNetworkReply *reply) {
            auto data = reply->readAll();
            auto doc = QJsonDocument::fromJson(data);
            auto obj = doc.object();

            if (obj.contains("error"_L1)) {
                Q_EMIT posted(obj["error"_L1].toString());
            } else {
                Q_EMIT posted(i18n("An unknown error occurred."));
            }
        },
        headers);
}

void PostEditorBackend::edit()
{
    QUrl edit_status_url = m_account->apiUrl(QStringLiteral("/api/v1/statuses/%1").arg(m_id));
    auto doc = toJsonDocument();

    m_account->put(edit_status_url, doc, true, this, [this](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();

        Q_EMIT editComplete(obj);
    });
}

void PostEditorBackend::saveDraft()
{
    // Set it far in the future so it's a "draft"
    auto currentDate = QDateTime::currentDateTime();
    currentDate.setDate(QDate(DRAFT_YEAR, currentDate.date().month(), currentDate.date().day()));
    setScheduledAt(currentDate);

    save();
}

void PostEditorBackend::loadScheduledPost(const QString &id)
{
    QUrl url = m_account->apiUrl(QStringLiteral("/api/v1/scheduled_statuses/%1").arg(id));

    m_account->get(url, true, this, [this, id](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();

        setStatus(obj["params"_L1].toObject()["text"_L1].toString());

        // Save this id for later, so we clean up the draft once we properly posted it.
        m_scheduledPostId = id;

        Q_EMIT scheduledPostLoaded();
    });
}

#include "moc_posteditorbackend.cpp"
