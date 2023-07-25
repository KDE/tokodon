// SPDX-FileCopyrightText: 2017 Konstantinos Sideris <siderisk@auth.gr>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "emojimodel.h"

#include <QVariant>

#include <KLocalizedString>

#include "account/abstractaccount.h"
#include "accountconfig.h"
#include "emojitones.h"

QHash<EmojiModel::Category, QVariantList> EmojiModel::_emojis;

EmojiModel::EmojiModel(QObject *parent)
    : QObject(parent)
{
    if (_emojis.isEmpty()) {
#include "emojis.h"
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

QVariantList EmojiModel::tones(const QString &baseEmoji) const
{
    if (baseEmoji.endsWith("tone")) {
        return EmojiTones::_tones.values(baseEmoji.split(":")[0]);
    }

    return EmojiTones::_tones.values(baseEmoji);
}

QStringList EmojiModel::history(AbstractAccount *account) const
{
    if (account == nullptr) {
        return {};
    }

    AccountConfig config(account->settingsGroupName());
    return config.lastUsedEmojis();
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

    AccountConfig config(account->settingsGroupName());
    config.setLastUsedEmojis(list.toList());
    config.save();

    Q_EMIT historyChanged();
}

QVariantList EmojiModel::categories() const
{
    return QVariantList{
        {QVariantMap{
            {"category", EmojiModel::History},
            {"name", i18nc("Previously used emojis", "History")},
            {"emoji", QStringLiteral("⌛️")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Custom},
            {"name", i18nc("'Custom' is a category of emoji", "Custom")},
            {"emoji", QStringLiteral("🖼️")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Smileys},
            {"name", i18nc("'Smileys' is a category of emoji", "Smileys")},
            {"emoji", QStringLiteral("😏")},
        }},
        {QVariantMap{
            {"category", EmojiModel::People},
            {"name", i18nc("'People' is a category of emoji", "People")},
            {"emoji", QStringLiteral("🙋‍♂️")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Nature},
            {"name", i18nc("'Nature' is a category of emoji", "Nature")},
            {"emoji", QStringLiteral("🌲")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Food},
            {"name", i18nc("'Food' is a category of emoji", "Food")},
            {"emoji", QStringLiteral("🍛")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Activities},
            {"name", i18nc("'Activities' is a category of emoji", "Activities")},
            {"emoji", QStringLiteral("🚁")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Travel},
            {"name", i18nc("'Travel' is  a category of emoji", "Travel")},
            {"emoji", QStringLiteral("🚅")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Objects},
            {"name", i18nc("'Objects' is a category of emoji", "Objects")},
            {"emoji", QStringLiteral("💡")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Symbols},
            {"name", i18nc("'Symbols' is a category of emoji", "Symbols")},
            {"emoji", QStringLiteral("🔣")},
        }},
        {QVariantMap{
            {"category", EmojiModel::Flags},
            {"name", i18nc("'Flags' is a category of emoji", "Flags")},
            {"emoji", QStringLiteral("🏁")},
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
