// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "card.h"
#include "datatype_p.h"

using namespace Qt::StringLiterals;

class PreviewCardAuthorPrivate : public QSharedData
{
public:
    QString name;
    QString description;
    QString author;
};

TOKODON_MAKE_GADGET(PreviewCardAuthor)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCardAuthor, QString, name)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCardAuthor, QString, description)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCardAuthor, QString, author)

PreviewCardAuthor PreviewCardAuthor::fromJson(const QJsonObject &object)
{
    PreviewCardAuthor previewCardAuthor;
    previewCardAuthor.d->name = object["name"_L1].toString();
    previewCardAuthor.d->description = object["description"_L1].toString();
    previewCardAuthor.d->author = object["author"_L1].toString();
    return previewCardAuthor;
}

class PreviewCardPrivate : public QSharedData
{
public:
    QUrl url;
    QString title;
    QString description;
    QString author;
};

TOKODON_MAKE_GADGET(PreviewCard)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QUrl, url)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, title)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, description)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, type)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QList<PreviewCardAuthor>, authors)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, providerName)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, providerUrl)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, html)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, int, width)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, int, height)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, image)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, embedUrl)
TOKODON_MAKE_PROPERTY_READONLY(PreviewCard, QString, blurhash)


PreviewCard PreviewCardAuthor::fromJson(const QJsonObject &object)
{
    PreviewCard previewCard;
    previewCard.d->url = object["url"_L1].toString();
    previewCard.d->title = object["title"_L1].toString();
    previewCard.d->description = object["description"_L1].toString();
    previewCard.d->title = object["title"_L1].toString();
    previewCard.d->authors = PreviewCardAuthor::fromJson(object["authors"_L1].toArray());
    previewCard.d->providerName = object["provider_name"_L1].toString();
    if (previewCard.d->providerName.isEmpty()) {
        previewCard.d->providerName = previewCard.url.host();
    }
    previewCard.d->providerUrl = object["provider_url"_L1].toString();
    previewCard.d->html = object["html"_L1].toString();
    previewCard.d->width = object["width"_L1].toInt();
    previewCard.d->height = object["height"_L1].toInt();
    previewCard.d->image = object["image"_L1].toString();
    previewCard.d->embedUrl = object["embed_url"_L1].toString();
    previewCard.d->blurhash = object["blurhash"_L1].toString();
    return previewCard;
}
