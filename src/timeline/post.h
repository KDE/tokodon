// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poll.h"
#include "timeline/attachment.h"

#include <QImage>

class Post;
class Identity;
class AbstractAccount;

class Application
{
    Q_GADGET

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QUrl website READ website)

public:
    Application() = default;
    explicit Application(QJsonObject application);

    QString name() const;
    QUrl website() const;

private:
    QJsonObject m_application;
};

class Card
{
    Q_GADGET

    Q_PROPERTY(QString authorName READ authorName)
    Q_PROPERTY(QString authorUrl READ authorUrl)
    Q_PROPERTY(QString blurhash READ blurhash)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString embedUrl READ embedUrl)
    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)
    Q_PROPERTY(QString html READ html)
    Q_PROPERTY(QString image READ image)
    Q_PROPERTY(QString providerName READ providerName)
    Q_PROPERTY(QString providerUrl READ providerUrl)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QUrl url READ url)

public:
    Card() = default;
    explicit Card(QJsonObject card);

    QString authorName() const;
    QString authorUrl() const;
    QString blurhash() const;
    QString description() const;
    QString embedUrl() const;
    int width() const;
    int height() const;
    QString html() const;
    QString image() const;
    QString providerName() const;
    QString providerUrl() const;
    QString title() const;
    QUrl url() const;

private:
    QJsonObject m_card;
};

class Post : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Posts should be accessed via models")

    Q_PROPERTY(QString spoilerText READ spoilerText WRITE setSpoilerText NOTIFY spoilerTextChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(bool sensitive READ sensitive WRITE setSensitive NOTIFY sensitiveChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)
    Q_PROPERTY(QStringList mentions READ mentions WRITE setMentions NOTIFY mentionsChanged)
    Q_PROPERTY(Poll *poll READ poll NOTIFY pollChanged)
    Q_PROPERTY(QStringList filters READ filters CONSTANT)
    Q_PROPERTY(Identity *authorIdentity READ getAuthorIdentity CONSTANT)
    Q_PROPERTY(QQmlListProperty<Attachment> attachments READ attachmentList CONSTANT)
    Q_PROPERTY(QString relativeTime READ relativeTime CONSTANT)
    Q_PROPERTY(QString absoluteTime READ absoluteTime CONSTANT)
    Q_PROPERTY(Card *card READ getCard CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(QString editedAt READ editedAt CONSTANT)
    Q_PROPERTY(bool wasEdited READ wasEdited CONSTANT)
    Q_PROPERTY(QList<QString> standaloneTags READ standaloneTags NOTIFY contentChanged)

public:
    Post() = delete;
    Post(const Post &) = delete;
    explicit Post(AbstractAccount *account, QObject *parent = nullptr);
    Post(AbstractAccount *account, QJsonObject obj, QObject *parent = nullptr);

    enum Visibility { Public, Unlisted, Private, Direct, Local };
    Q_ENUM(Visibility)

    AbstractAccount *m_parent;

    QString type() const;
    void fromJson(QJsonObject obj);

    Identity *getAuthorIdentity() const;
    std::shared_ptr<Identity> authorIdentity() const;
    std::shared_ptr<Identity> boostIdentity() const;
    std::shared_ptr<Identity> replyIdentity() const;
    bool boosted() const;

    /// Returns the post id of the status itself
    QString postId() const;
    QString originalPostId() const;
    QUrl url() const;

    /// Returns the spoiler text (subject) of the status
    QString spoilerText() const;
    void setSpoilerText(const QString &spoilerText);

    QString content() const;
    void setContent(const QString &content);
    QVector<QString> standaloneTags() const;
    QString contentType() const;
    void setContentType(const QString &contentType);
    bool sensitive() const;
    void setSensitive(bool isSensitive);
    Visibility visibility() const;
    void setVisibility(Visibility visibility);
    QString language() const;
    void setLanguage(const QString &language);
    QString inReplyTo() const;
    void setInReplyTo(const QString &inReplyTo);
    QStringList mentions() const;
    void setMentions(const QStringList &mentions);
    std::optional<Card> card() const;
    Card *getCard() const;
    void setCard(std::optional<Card> card);
    std::optional<Application> application() const;
    void setApplication(std::optional<Application> application);
    void setPollJson(const QJsonObject &object);
    Poll *poll() const;
    QStringList filters() const;

    void addAttachment(const QJsonObject &attachment);

    /// Returns the published/creation time of this status.
    QDateTime publishedAt() const;

    /// Returns a locale-aware relative time.
    QString relativeTime() const;

    // Returns absolute locale-aware time
    QString absoluteTime() const;

    /// Returns the time this post was last edited
    QString editedAt() const;

    /// Returns if the post was edited at all
    bool wasEdited() const;

    /// Returns whether the user favorited this status.
    bool favourited() const;
    /// Set this post as favourited.
    void setFavourited(bool favourited);
    /// Returns whether the user reblogged this status.
    bool reblogged() const;
    /// Set this post as reblogged.
    void setReblogged(bool reblogged);
    /// Returns whether the user muted this status.
    bool muted() const;
    /// Set this post as muted.
    void setMuted(bool muted);
    /// Returns whether the user bookmarked this status.
    bool bookmarked() const;
    /// Set this post as bookmarked.
    void setBookmarked(bool bookmarked);
    /// Returns whether the user pinned this status.
    bool pinned() const;
    /// Set this status as pinned
    void setPinned(bool pinned);
    /// Returns whether the user filtered this status.
    bool filtered() const;

    /// Returns the number of time this status has been boosted.
    int reblogsCount() const;
    /// Returns the number of time this status has been favorited.
    int favouritesCount() const;
    /// Returns the number of time this status has been replied.
    int repliesCount() const;

    /// Returns whether this status is empty
    bool isEmpty() const;

    Q_INVOKABLE void addAttachments(const QJsonArray &attachments);
    void setDirtyAttachment();
    void updateAttachment(Attachment *a);
    QList<Attachment *> attachments() const;
    QQmlListProperty<Attachment> attachmentList() const;
    bool attachmentsVisible() const;
    void setAttachmentsVisible(bool attachmentsVisible);

    bool hidden() const
    {
        return m_hidden;
    }

    static QString visibilityToString(Post::Visibility visibility);

    static Post::Visibility stringToVisibility(const QString &visibility);

Q_SIGNALS:
    void spoilerTextChanged();
    void contentChanged();
    void contentTypeChanged();
    void sensitiveChanged();
    void visibilityChanged();
    void languageChanged();
    void inReplyToChanged();
    void mentionsChanged();
    void attachmentUploaded();
    void pollChanged();
    void replyIdentityChanged();

private:
    void processContent(const QJsonObject &obj);

    bool m_attachments_visible = true;
    QDateTime m_publishedAt;
    QString m_postId;
    QString m_originalPostId;
    QUrl m_url;
    QString m_content;
    QString m_spoilerText;
    QString m_author;
    QString m_reply_to_author;
    QString m_content_type;
    QStringList m_mentions;
    QString m_language;
    QDateTime m_editedAt;
    QVector<QString> m_standaloneTags;

    QString m_replyTargetId;
    QStringList m_filters;
    std::optional<Card> m_card;
    std::optional<Application> m_application;
    std::shared_ptr<Identity> m_authorIdentity;
    QList<Attachment *> m_attachments;
    QQmlListProperty<Attachment> m_attachmentList;
    std::unique_ptr<Poll> m_poll;

    bool m_sensitive = false;
    Visibility m_visibility;

    bool m_boosted = false;
    std::shared_ptr<Identity> m_boostIdentity;

    std::shared_ptr<Identity> m_replyIdentity;

    bool m_favourited = false;
    bool m_reblogged = false;
    bool m_muted = false;
    bool m_bookmarked = false;
    bool m_filtered = false;
    bool m_hidden = false;
    bool m_pinned = false;

    int m_reblogsCount = 0;
    int m_favouritesCount = 0;
    int m_repliesCount = 0;
};
