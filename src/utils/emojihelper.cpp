// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "emojihelper.h"

#include "account/abstractaccount.h"

#include <TextEmoticonsCore/EmojiModel>
#include <TextEmoticonsCore/EmojiModelManager>
#include <TextEmoticonsCore/EmoticonUnicodeUtils>

EmojiHelper &EmojiHelper::instance()
{
    static EmojiHelper emojiHelper;
    return emojiHelper;
}

QList<TextEmoticonsCore::EmoticonCategory> EmojiHelper::categories() const
{
    auto categories = TextEmoticonsCore::UnicodeEmoticonManager::self()->categories();

    // Custom
    TextEmoticonsCore::EmoticonCategory customCategory;
    customCategory.setName(TextEmoticonsCore::EmoticonUnicodeUtils::customName());
    customCategory.setCategory(TextEmoticonsCore::EmoticonUnicodeUtils::customIdentifier());
    categories.push_front(customCategory);

    // Recents
    TextEmoticonsCore::EmoticonCategory recentsCategory;
    recentsCategory.setName(TextEmoticonsCore::EmoticonUnicodeUtils::recentName());
    recentsCategory.setCategory(TextEmoticonsCore::EmoticonUnicodeUtils::recentIdentifier());
    categories.push_front(recentsCategory);

    return categories;
}

void EmojiHelper::setupCustomEmoji(AbstractAccount *account)
{
    auto model = TextEmoticonsCore::EmojiModelManager::self()->emojiModel();
    auto mastodonEmoji = account->customEmojis();
    QList<TextEmoticonsCore::CustomEmoji> emoji;
    std::ranges::transform(std::as_const(mastodonEmoji), std::back_inserter(emoji), [](const CustomEmoji &mastodonEmoji) {
        TextEmoticonsCore::CustomEmoji customEmoji;
        customEmoji.setIdentifier(mastodonEmoji.shortcode);
        customEmoji.setFileName(mastodonEmoji.url);
        return customEmoji;
    });
    TextEmoticonsCore::EmojiModelManager::self()->setRecentSettingsGroupName(account->settingsGroupName());
    model->setCustomEmojiList(emoji);
}
