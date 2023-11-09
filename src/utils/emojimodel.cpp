// SPDX-FileCopyrightText: 2017 Konstantinos Sideris <siderisk@auth.gr>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "emojimodel.h"

#include <KLocalizedString>
#include <TextEmoticonsCore/EmojiModel>
#include <TextEmoticonsCore/UnicodeEmoticonManager>

#include "abstractaccount.h"

using namespace Qt::Literals::StringLiterals;

QHash<EmojiModel::Category, QVariantList> EmojiModel::_emojis;

EmojiModel::EmojiModel(QObject *parent)
    : QObject(parent)
{
    m_emojiManager = new TextEmoticonsCore::EmojiModelManager(this);
    m_emojiProxyModel = new TextEmoticonsCore::EmojiProxyModel(this);
    m_emojiProxyModel->setSourceModel(TextEmoticonsCore::EmojiModelManager::self()->emojiModel());

    connect(this, &EmojiModel::categoryChanged, this, [this] {
        m_emojiProxyModel->setCategory(m_category);
    });

    connect(this, &EmojiModel::searchStringChanged, this, [this] {
        m_emojiProxyModel->setSearchIdentifier(m_searchString);
    });
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
    /*if (baseEmoji.endsWith("tone"_L1)) {
        return EmojiTones::_tones.values(baseEmoji.split(":"_L1)[0]);
    }

    return EmojiTones::_tones.values(baseEmoji);*/
    return {};
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

QList<TextEmoticonsCore::EmoticonCategory> EmojiModel::categories() const
{
    return TextEmoticonsCore::UnicodeEmoticonManager::self()->categories();
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
