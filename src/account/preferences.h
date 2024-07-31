// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/post.h"

/**
 * @brief Account-specific preferences which are stored server-side.
 */
class Preferences final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Post::Visibility defaultVisibility READ defaultVisibility WRITE setDefaultVisibility NOTIFY defaultVisibilityChanged)
    Q_PROPERTY(bool defaultSensitive READ defaultSensitive WRITE setDefaultSensitive NOTIFY defaultSensitiveChanged)
    Q_PROPERTY(QString defaultLanguage READ defaultLanguage WRITE setDefaultLanguage NOTIFY defaultLanguageChanged)
    Q_PROPERTY(QString extendMedia READ extendMedia NOTIFY extendMediaChanged)
    Q_PROPERTY(bool extendSpoiler READ extendSpoiler NOTIFY extendSpoilerChanged)

public:
    explicit Preferences(AbstractAccount *account);

    /**
     * @return The default visibility when composing new statuses.
     * @see setDefaultVisibility()
     */
    Post::Visibility defaultVisibility() const;

    /**
     * @brief Sets the default visibility when composing new statuses.
     * @param visibility The new default visibility.
     * @see defaultVisibility()
     */
    void setDefaultVisibility(Post::Visibility visibility);

    /**
     * @return Whether newly uploaded media attachments are sensitive by default.
     * @see setDefaultSensitive()
     */
    bool defaultSensitive() const;

    /**
     * @brief Set whether newly uploaded media attachments are sensitive by default.
     * @param sensitive Whether or not media attachments are sensitive by default.
     * @see defaultSensitive()
     */
    void setDefaultSensitive(bool sensitive);

    /**
     * @return The default language code (ISO 6391) when composing new statuses.
     * @see setDefaultLanguage()
     */
    QString defaultLanguage() const;

    /**
     * @brief Set the default language code (ISO 6391) when composing new statuses.
     * @param language The new default language code.
     * @see defaultLanguage()
     */
    void setDefaultLanguage(const QString &language);

    /**
     * @return If media should not be cropped to 16:9.
     * @note Currently read-only, see https://github.com/mastodon/mastodon/issues/7021
     */
    QString extendMedia() const;

    /**
     * @return If spoilers on statuses should be ignored, and always shown.
     * @note Currently read-only, see https://github.com/mastodon/mastodon/issues/7021
     */
    bool extendSpoiler() const;

Q_SIGNALS:
    /**
     * @brief Emitted when the default status visibility preference has been changed.
     * @see setDefaultVisibility()
     */
    void defaultVisibilityChanged();

    /**
     * @brief Emitted when the default media attachment sensitivity preference has been changed.
     * @see setDefaultSensitive()
     */
    void defaultSensitiveChanged();

    /**
     * @brief Emitted when the default status language preference has been changed.
     * @see setDefaultLanguage()
     */
    void defaultLanguageChanged();

    /**
     * @brief Emitted when the extend media preference has been changed.
     */
    void extendMediaChanged();

    /**
     * @brief Emitted when the extend spoiler preference has been changed.
     */
    void extendSpoilerChanged();

private:
    void setPreferencesField(const QString &name, const QString &value);

    AbstractAccount *const m_account;

    Post::Visibility m_defaultVisibility;
    bool m_defaultSensitive;
    QString m_defaultLanguage;
    QString m_extendMedia;
    bool m_extendSpoiler;
};
