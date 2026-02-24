// SPDX-FileCopyrightText: 2017 Konstantinos Sideris <siderisk@auth.gr>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils/emojimodel.h"

#include <KLocalizedString>

#include "account/abstractaccount.h"
#include "emojitones.h"

struct {
    EmojiModel::Category category;
    const char8_t *escaped_sequence;
    const char8_t *shortcode;
    const char8_t *description;
} constexpr const emoji_data[] = {
#include "emojis.h"
};

using namespace Qt::Literals::StringLiterals;

QHash<EmojiModel::Category, QVariantList> EmojiModel::_emojis;

EmojiModel::EmojiModel(QObject *parent)
    : QObject(parent)
{
    if (_emojis.isEmpty()) {
        for (const auto &emoji : emoji_data) {
            _emojis[emoji.category].push_back(QVariant::fromValue(
                Emoji(QString::fromUtf8(emoji.escaped_sequence), QString::fromUtf8(emoji.shortcode), QString::fromUtf8(emoji.description))));
        }
    }
}

QVariantList EmojiModel::filterModel(AbstractAccount *account, const QString &filter)
{
    return filterCustomModel(account, filter) + filterModelNoCustom(filter);
}

QVariantList EmojiModel::emojis(AbstractAccount *account, Category category) const
{
    if (category == History) {
        QVariantList list;
        for (const auto &historicEmoji : history(account)) {
            for (const auto &emojiCategory : _emojis) {
                for (const auto &emoji : emojiCategory) {
                    if (qvariant_cast<Emoji>(emoji).shortName == historicEmoji) {
                        list.append(emoji);
                    }
                }
            }

            for (const auto &customEmoji : filterCustomModel(account, {})) {
                if (qvariant_cast<CustomEmoji>(customEmoji).shortcode == historicEmoji) {
                    list.append(customEmoji);
                }
            }
        }

        return list;
    } else if (category == Custom) {
        return filterCustomModel(account, {});
    }

    return _emojis[category];
}

QList<Emoji> EmojiModel::tones(const QString &baseEmoji) const
{
    if (baseEmoji.endsWith("tone"_L1)) {
        return EmojiTones::tones().values(baseEmoji.split(":"_L1)[0]);
    }

    return EmojiTones::tones().values(baseEmoji);
}

QStringList EmojiModel::history(AbstractAccount *account) const
{
    if (account == nullptr) {
        return {};
    }

    return account->config()->lastUsedEmojis();
}

QVariantList EmojiModel::filterModelNoCustom(const QString &filter)
{
    QVariantList result;

    for (const auto &e : _emojis.values()) {
        for (const auto &variant : e) {
            const auto &emoji = qvariant_cast<Emoji>(variant);
            if (emoji.shortName.contains(filter, Qt::CaseInsensitive)) {
                result.append(variant);
            }
        }
    }

    return result;
}

void EmojiModel::emojiUsed(AbstractAccount *account, const QString &shortcode)
{
    if (account == nullptr) {
        return;
    }

    auto list = history(account).toVector();

    // Remove previous instances of this emoji
    auto it = list.begin();
    while (it != list.end()) {
        if ((*it) == shortcode) {
            it = list.erase(it);
        } else {
            it++;
        }
    }

    list.push_front(shortcode);

    // sigh, QList didn't have resize until 6.0.
    if (list.size() > 100) {
        list.resize(100);
    }

    account->config()->setLastUsedEmojis(list.toList());
    account->config()->save();

    Q_EMIT historyChanged();
}

QVariantList EmojiModel::categories() const
{
    return QVariantList{
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::History},
            {QStringLiteral("name"), i18nc("Previously used emojis", "History")},
            {QStringLiteral("emoji"), QStringLiteral("⌛️")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Custom},
            {QStringLiteral("name"), i18nc("'Custom' is a category of emoji", "Custom")},
            {QStringLiteral("emoji"), QStringLiteral("🖼️")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Smileys},
            {QStringLiteral("name"), i18nc("'Smileys' is a category of emoji", "Smileys")},
            {QStringLiteral("emoji"), QStringLiteral("😏")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::People},
            {QStringLiteral("name"), i18nc("'People' is a category of emoji", "People")},
            {QStringLiteral("emoji"), QStringLiteral("🙋‍♂️")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Nature},
            {QStringLiteral("name"), i18nc("'Nature' is a category of emoji", "Nature")},
            {QStringLiteral("emoji"), QStringLiteral("🌲")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Food},
            {QStringLiteral("name"), i18nc("'Food' is a category of emoji", "Food")},
            {QStringLiteral("emoji"), QStringLiteral("🍛")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Activities},
            {QStringLiteral("name"), i18nc("'Activities' is a category of emoji", "Activities")},
            {QStringLiteral("emoji"), QStringLiteral("🚁")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Travel},
            {QStringLiteral("name"), i18nc("'Travel' is  a category of emoji", "Travel")},
            {QStringLiteral("emoji"), QStringLiteral("🚅")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Objects},
            {QStringLiteral("name"), i18nc("'Objects' is a category of emoji", "Objects")},
            {QStringLiteral("emoji"), QStringLiteral("💡")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Symbols},
            {QStringLiteral("name"), i18nc("'Symbols' is a category of emoji", "Symbols")},
            {QStringLiteral("emoji"), QStringLiteral("🔣")},
        }},
        {QVariantMap{
            {QStringLiteral("category"), EmojiModel::Flags},
            {QStringLiteral("name"), i18nc("'Flags' is a category of emoji", "Flags")},
            {QStringLiteral("emoji"), QStringLiteral("🏁")},
        }},
    };
}

QVariantList EmojiModel::filterCustomModel(AbstractAccount *account, const QString &filter)
{
    if (account == nullptr) {
        return {};
    }

    QVariantList result;
    for (const auto &emoji : account->customEmojis()) {
        if (emoji.shortcode.contains(filter, Qt::CaseInsensitive)) {
            result.append(QVariant::fromValue(emoji));
        }
    }

    return result;
}

#include "moc_emojimodel.cpp"
