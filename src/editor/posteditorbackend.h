// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "editor/polleditorbackend.h"
#include "timeline/post.h"

class AttachmentEditorModel;

class PostEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)
    Q_PROPERTY(QString spoilerText READ spoilerText WRITE setSpoilerText NOTIFY spoilerTextChanged)
    Q_PROPERTY(Post::Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QDateTime scheduledAt READ scheduledAt WRITE setScheduledAt NOTIFY scheduledAtChanged)
    Q_PROPERTY(QStringList mentions READ mentions WRITE setMentions NOTIFY mentionsChanged)
    Q_PROPERTY(AttachmentEditorModel *attachmentEditorModel READ attachmentEditorModel CONSTANT)
    Q_PROPERTY(bool sensitive READ sensitive WRITE setSensitive NOTIFY sensitiveChanged)
    Q_PROPERTY(PollEditorBackend *poll MEMBER m_poll CONSTANT)
    Q_PROPERTY(bool pollEnabled MEMBER m_pollEnabled NOTIFY pollEnabledChanged)
    Q_PROPERTY(int charactersLeft READ charactersLeft NOTIFY statusChanged)

    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)

public:
    explicit PostEditorBackend(QObject *parent = nullptr);
    ~PostEditorBackend() override;

    QString id() const;
    void setId(const QString &id);

    QString status() const;
    void setStatus(const QString &status);

    QString spoilerText() const;
    void setSpoilerText(const QString &spoilerText);

    QString inReplyTo() const;
    void setInReplyTo(const QString &inReplyTo);

    Post::Visibility visibility() const;
    void setVisibility(Post::Visibility visibility);

    QString language() const;
    void setLanguage(const QString &language);

    QDateTime scheduledAt() const;
    void setScheduledAt(const QDateTime &scheduledAt);

    QStringList mentions() const;
    void setMentions(const QStringList &mentions);

    AttachmentEditorModel *attachmentEditorModel() const;

    bool sensitive() const;
    void setSensitive(bool sensitive);

    AbstractAccount *account() const;
    void setAccount(AbstractAccount *account);

    int charactersLeft() const;

    Q_INVOKABLE void copyFromOther(PostEditorBackend *other);

public Q_SLOTS:
    void save();
    void edit();

Q_SIGNALS:

    void statusChanged();

    void spoilerTextChanged();

    void inReplyToChanged();

    void visibilityChanged();

    void languageChanged();

    void scheduledAtChanged();

    void mentionsChanged();

    void accountChanged();

    void sensitiveChanged();
    void editComplete(QJsonObject object);

    void posted(QString error);

    void pollEnabledChanged();

private:
    QJsonDocument toJsonDocument() const;

    QString m_id;
    QString m_status;
    QString m_idenpotencyKey;
    QString m_spoilerText;
    QString m_inReplyTo;
    QString m_language;
    QDateTime m_scheduledAt;
    QStringList m_mentions;
    bool m_sensitive = false;
    PollEditorBackend *m_poll = nullptr;
    bool m_pollEnabled = false;
    Post::Visibility m_visibility = Post::Visibility::Public;
    AbstractAccount *m_account = nullptr;
    AttachmentEditorModel *m_attachmentEditorModel = nullptr;
};
