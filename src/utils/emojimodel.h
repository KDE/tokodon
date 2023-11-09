// SPDX-FileCopyrightText: 2018 Black Hat <bhat@encom.eu.org>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtQml>

/**
 * @class Emoji
 *
 * Defines the structure of a typical Unicode emoji.
 */
struct Emoji {
    Emoji(QString unicode, QString shortname, bool isCustom = false)
        : unicode(std::move(unicode))
        , shortName(std::move(shortname))
        , isCustom(isCustom)
    {
    }
    Emoji(QString unicode, QString shortname, QString description)
        : unicode(std::move(unicode))
        , shortName(std::move(shortname))
        , description(std::move(description))
    {
    }
    Emoji() = default;

    QString unicode;
    QString shortName;
    QString description;
    bool isCustom = false;

    Q_GADGET

    Q_PROPERTY(QString unicode MEMBER unicode)
    Q_PROPERTY(QString shortName MEMBER shortName)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(bool isCustom MEMBER isCustom)
};

class AbstractAccount;

/**
 * @class EmojiModel
 *
 * This class defines the model for visualising a list of emojis.
 */
class EmojiModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    /**
     * @brief Return a list of emoji categories.
     */
    Q_PROPERTY(QVariantList categories READ categories CONSTANT)

public:
    explicit EmojiModel(QObject *parent = nullptr);

    /**
     * @brief Defines the potential categories an emoji can be placed in.
     */
    enum Category {
        Custom, /**< A custom user emoji. */
        Search, /**< The results of a filter. */
        History, /**< Recently used emojis. */
        Smileys, /**< Smileys & emotion emojis. */
        People, /**< People & Body emojis. */
        Nature, /**< Animals & Nature emojis. */
        Food, /**< Food & Drink emojis. */
        Activities, /**< Activities emojis. */
        Travel, /**< Travel & Places emojis. */
        Objects, /**< Objects emojis. */
        Symbols, /**< Symbols emojis. */
        Flags, /**< Flags emojis. */
        Component, /**< ??? */
    };
    Q_ENUM(Category)

    /**
     * @brief Return a filtered list of emojis.
     *
     * @note This includes custom emojis, use filterModelNoCustom to return a result
     *       without custom emojis.
     *
     * @sa filterModelNoCustom
     */
    Q_INVOKABLE static QVariantList filterModel(AbstractAccount *account, const QString &filter);

    /**
     * @brief Return a list of emojis for the given category.
     */
    Q_INVOKABLE QVariantList emojis(AbstractAccount *account, Category category) const;

    /**
     * @brief Return a list of emoji tones for the given base emoji.
     */
    Q_INVOKABLE QVariantList tones(const QString &baseEmoji) const;

    /**
     * @brief Return a list of emoji that were recently used.
     */
    Q_INVOKABLE QStringList history(AbstractAccount *account) const;

Q_SIGNALS:
    void historyChanged();

public Q_SLOTS:
    void emojiUsed(AbstractAccount *account, const QString &shortcode);

private:
    static QHash<Category, QVariantList> _emojis;

    QVariantList categories() const;

    static QVariantList filterModelNoCustom(const QString &filter);
    static QVariantList filterCustomModel(AbstractAccount *account, const QString &filter);
};
