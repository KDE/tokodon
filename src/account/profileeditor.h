// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QtQml>

class AbstractAccount;

/// Class responsible for editing the account personal information and preferences
class ProfileEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString displayNameHtml READ displayNameHtml NOTIFY displayNameChanged)
    Q_PROPERTY(QString note READ note WRITE setNote NOTIFY noteChanged)
    Q_PROPERTY(bool bot READ bot WRITE setBot NOTIFY botChanged)
    Q_PROPERTY(QUrl backgroundUrl READ backgroundUrl WRITE setBackgroundUrl NOTIFY backgroundUrlChanged)
    Q_PROPERTY(QUrl avatarUrl READ avatarUrl WRITE setAvatarUrl NOTIFY avatarUrlChanged)
    Q_PROPERTY(QString backgroundUrlError READ backgroundUrlError NOTIFY backgroundUrlChanged)
    Q_PROPERTY(QString avatarUrlError READ avatarUrlError NOTIFY avatarUrlChanged)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(bool discoverable READ discoverable WRITE setDiscoverable NOTIFY discoverableChanged)

public:
    explicit ProfileEditorBackend(QObject *parent = nullptr);
    ~ProfileEditorBackend() override;

    /// Returns the account we're updating
    AbstractAccount *account() const;

    /// Sets the account to edit
    /// \param account The account to edit
    void setAccount(AbstractAccount *account);

    /// Returns the display name of the account
    QString displayName() const;

    /// Returns the display name of the account, processed to HTML (for custom emojis)
    QString displayNameHtml() const;

    /// Sets a new display name for the account
    /// \param displayName The new display name, should be plain text
    void setDisplayName(const QString &displayName);

    /// Returns the biography text for the account
    QString note() const;

    /// Sets a new biography text for the account
    /// \param note The new bio
    void setNote(const QString &note);

    /// Returns if the account should be marked as a bot
    bool bot() const;

    /// Sets if the account should be marked as a bot
    /// \param bot If true, the account is marked as a bot and will be visible on the profile
    void setBot(bool bot);

    /// Returns the current avatar URL for the account
    QUrl avatarUrl() const;

    /// Returns a localized error if the new avatar failed to upload
    QString avatarUrlError() const;

    /// Sets a new avatar URL for the account
    /// \param avatarUrl The new media URL for the avatar
    void setAvatarUrl(const QUrl &avatarUrl);

    /// Returns the current background URL for the account
    QUrl backgroundUrl() const;

    /// Returns a localized error if the new background failed to upload
    QString backgroundUrlError() const;

    /// Sets a new background URL for the account
    /// \param backgroundUrl The new media URL for the background
    void setBackgroundUrl(const QUrl &backgroundUrl);

    /// Whether the account should be discoverable on the server directory
    bool discoverable() const;

    /// Sets whether the account should be discoverable on the server directory
    /// \param discoverable If set to true, the account will be listed on the server's directory
    void setDiscoverable(bool discoverable);

    /// Whether the account is locked
    bool locked() const;

    /// Sets if the account is locked or not
    /// \param locked If true, the account is locked
    void setLocked(bool locked);

public Q_SLOTS:
    void save();
    void fetchAccountInfo();

Q_SIGNALS:
    void accountChanged();
    void displayNameChanged();
    void noteChanged();
    void botChanged();
    void avatarUrlChanged();
    void backgroundUrlChanged();
    void discoverableChanged();
    void lockedChanged();
    void sendNotification(const QString &message, const QString &type = QStringLiteral("info"));

private:
    AbstractAccount *m_account = nullptr;
    QString m_displayName;
    QString m_note;
    bool m_bot = false;
    QUrl m_avatarUrl;
    QUrl m_backgroundUrl;
    bool m_discoverable = true;
    bool m_locked = false;
};
