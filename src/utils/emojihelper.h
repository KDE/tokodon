// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QQmlEngine>
#include <TextEmoticonsCore/EmojiModelManager>
#include <TextEmoticonsCore/UnicodeEmoticonManager>

class AbstractAccount;

/**
 * @brief Deals with emoji.
 */
class EmojiHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QList<TextEmoticonsCore::EmoticonCategory> categories READ categories CONSTANT)

public:
    static EmojiHelper *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    static EmojiHelper &instance();

    /// KTextAddons doesn't have custom/recent in it's category list, that's left up to the application
    QList<TextEmoticonsCore::EmoticonCategory> categories() const;

    void setupCustomEmoji(AbstractAccount *account);

private:
    EmojiHelper() = default;
};
