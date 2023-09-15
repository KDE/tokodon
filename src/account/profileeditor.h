// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class AbstractAccount;

/// Class responsible for editing the account personal information and preferences
class ProfileEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /// This property holds the account we want to update.
    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)

    /// This property holds the display name of the account.
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)

    /// This property holds the display name of the account in rendered HTML form.
    Q_PROPERTY(QString displayNameHtml READ displayNameHtml NOTIFY displayNameChanged)

    /// This property holds the note (bio) of the account.
    Q_PROPERTY(QString note READ note WRITE setNote NOTIFY noteChanged)

    /// This property holds the default post privacy to be used for new statuses.
    Q_PROPERTY(QString privacy READ privacy WRITE setPrivacy NOTIFY privacyChanged)

    /// This property holds whether new statuses should be marked sensitive by default.
    Q_PROPERTY(bool sensitive READ sensitive WRITE setSensitive NOTIFY sensitiveChanged)

    /// This property holds whether this account is a bot.
    Q_PROPERTY(bool bot READ bot WRITE setBot NOTIFY botChanged)

    /// This property holds the the image banner that is shown above the profile
    /// and in profile cards.
    Q_PROPERTY(QUrl backgroundUrl READ backgroundUrl WRITE setBackgroundUrl NOTIFY backgroundUrlChanged)

    /// This property holds the the image banner that is shown above the profile
    /// and in profile cards.
    Q_PROPERTY(QUrl avatarUrl READ avatarUrl WRITE setAvatarUrl NOTIFY avatarUrlChanged)

    /// This property holds whether the current backgroundUrl has an error
    Q_PROPERTY(QString backgroundUrlError READ backgroundUrlError NOTIFY backgroundUrlChanged)

    /// This property holds whether the current avatarUrl has an error
    Q_PROPERTY(QString avatarUrlError READ avatarUrlError NOTIFY avatarUrlChanged)

    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)

    Q_PROPERTY(bool discoverable READ discoverable WRITE setDiscoverable NOTIFY discoverableChanged)

public:
    explicit ProfileEditorBackend(QObject *parent = nullptr);
    ~ProfileEditorBackend() override;

    AbstractAccount *account() const;
    void setAccount(AbstractAccount *account);

    QString displayName() const;
    QString displayNameHtml() const;
    void setDisplayName(const QString &displayName);

    QString note() const;
    void setNote(const QString &note);

    bool sensitive() const;
    void setSensitive(bool sensitive);

    QString privacy() const;
    void setPrivacy(const QString &privacy);

    bool bot() const;
    void setBot(bool bot);

    QString language() const;
    void setLanguage(const QString &language);

    QUrl avatarUrl() const;
    QString avatarUrlError() const;
    void setAvatarUrl(const QUrl &avatarUrl);

    QUrl backgroundUrl() const;
    QString backgroundUrlError() const;
    void setBackgroundUrl(const QUrl &backgroundUrl);

    bool discoverable() const;
    void setDiscoverable(bool discoverable);

    bool locked() const;
    void setLocked(bool locked);

public Q_SLOTS:
    void save();
    void fetchAccountInfo();

Q_SIGNALS:
    void accountChanged();
    void displayNameChanged();
    void noteChanged();
    void sensitiveChanged();
    void privacyChanged();
    void botChanged();
    void languageChanged();
    void avatarUrlChanged();
    void backgroundUrlChanged();
    void discoverableChanged();
    void lockedChanged();
    void sendNotification(const QString &message, const QString &type = QStringLiteral("info"));

private:
    AbstractAccount *m_account = nullptr;
    QString m_displayName;
    QString m_note;
    QString m_privacy = QStringLiteral("public");
    bool m_sensitive = false;
    bool m_bot = false;
    QString m_language;
    QUrl m_avatarUrl;
    QUrl m_backgroundUrl;
    bool m_discoverable = true;
    bool m_locked = false;
};
