// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "profileeditor.h"

#include "abstractaccount.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

ProfileEditorBackend::ProfileEditorBackend(QObject *parent)
    : QObject(parent)
{
}

ProfileEditorBackend::~ProfileEditorBackend() = default;

AbstractAccount *ProfileEditorBackend::account() const
{
    return m_account;
}

void ProfileEditorBackend::setAccount(AbstractAccount *account)
{
    if (m_account == account) {
        return;
    }
    m_account = account;
    Q_EMIT accountChanged();
    if (m_account) {
        fetchAccountInfo();
    }
}

QString ProfileEditorBackend::displayName() const
{
    return m_displayName;
}

QString ProfileEditorBackend::displayNameHtml() const
{
    return CustomEmoji::replaceCustomEmojis(m_account->customEmojis(), m_displayName);
}

void ProfileEditorBackend::setDisplayName(const QString &displayName)
{
    if (m_displayName == displayName) {
        return;
    }
    m_displayName = displayName;
    Q_EMIT displayNameChanged();
}

QString ProfileEditorBackend::note() const
{
    return m_note;
}

void ProfileEditorBackend::setNote(const QString &note)
{
    if (m_note == note) {
        return;
    }
    m_note = note;
    Q_EMIT noteChanged();
}

bool ProfileEditorBackend::bot() const
{
    return m_bot;
}

void ProfileEditorBackend::setBot(bool bot)
{
    if (m_bot == bot) {
        return;
    }
    m_bot = bot;
    Q_EMIT botChanged();
}

bool ProfileEditorBackend::discoverable() const
{
    return m_discoverable;
}

void ProfileEditorBackend::setDiscoverable(bool discoverable)
{
    if (m_discoverable == discoverable) {
        return;
    }
    m_discoverable = discoverable;
    Q_EMIT discoverableChanged();
}

bool ProfileEditorBackend::locked() const
{
    return m_locked;
}

void ProfileEditorBackend::setLocked(bool locked)
{
    if (m_locked == locked) {
        return;
    }
    m_locked = locked;
    Q_EMIT lockedChanged();
}

QUrl ProfileEditorBackend::avatarUrl() const
{
    return m_avatarUrl;
}

void ProfileEditorBackend::setAvatarUrl(const QUrl &avatarUrl)
{
    if (avatarUrl == m_avatarUrl) {
        return;
    }
    m_avatarUrl = avatarUrl;
    Q_EMIT avatarUrlChanged();
}

QUrl ProfileEditorBackend::backgroundUrl() const
{
    return m_backgroundUrl;
}

namespace
{

auto operator""_MiB(unsigned long long const x) -> long
{
    return 1024L * 1024L * x;
}

QString checkImage(const QUrl &url)
{
    if (!url.isLocalFile() || url.isEmpty()) {
        return {};
    }

    QFileInfo fileInfo(url.toLocalFile());
    if (fileInfo.size() > 2_MiB) {
        return i18n("Image is too big");
    }

    QMimeDatabase mimeDatabase;
    const auto mimeType = mimeDatabase.mimeTypeForFile(fileInfo);

    static const QSet<QString> allowedMimeType = {
        QStringLiteral("image/png"),
        QStringLiteral("image/jpeg"),
        QStringLiteral("image/gif"),
    };

    if (!allowedMimeType.contains(mimeType.name())) {
        return i18n("Unsupported image file. Only jpeg, png and gif are supported.");
    }

    return {};
}
}

QString ProfileEditorBackend::backgroundUrlError() const
{
    return checkImage(m_backgroundUrl);
}

QString ProfileEditorBackend::avatarUrlError() const
{
    return checkImage(m_avatarUrl);
}

void ProfileEditorBackend::setBackgroundUrl(const QUrl &backgroundUrl)
{
    if (backgroundUrl == m_backgroundUrl) {
        return;
    }
    m_backgroundUrl = backgroundUrl;
    Q_EMIT backgroundUrlChanged();
}

void ProfileEditorBackend::fetchAccountInfo()
{
    Q_ASSERT_X(m_account, Q_FUNC_INFO, "Fetch account called without account");

    m_account->get(m_account->apiUrl(QStringLiteral("/api/v1/accounts/verify_credentials")), true, this, [this](QNetworkReply *reply) {
        const auto json = QJsonDocument::fromJson(reply->readAll());
        Q_ASSERT(json.isObject());
        const auto obj = json.object();
        setDisplayName(obj["display_name"_L1].toString());
        const auto source = obj["source"_L1].toObject();
        setNote(source["note"_L1].toString());
        setBot(obj["bot"_L1].toBool());
        setBackgroundUrl(QUrl(obj["header_static"_L1].toString()));
        setAvatarUrl(QUrl(obj["avatar_static"_L1].toString()));
        setLocked(obj["locked"_L1].toBool());
        setDiscoverable(obj["discoverable"_L1].toBool());
    });
}

void ProfileEditorBackend::save()
{
    const QUrl url = m_account->apiUrl(QStringLiteral("/api/v1/accounts/update_credentials"));

    auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart displayNamePart;
    displayNamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"display_name\""));
    displayNamePart.setBody(displayName().toUtf8());
    multiPart->append(displayNamePart);

    QHttpPart notePart;
    notePart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"note\""));
    notePart.setBody(note().toUtf8());
    multiPart->append(notePart);

    QHttpPart lockedPart;
    lockedPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"locked\""));
    lockedPart.setBody(locked() ? "1" : "0");
    multiPart->append(lockedPart);

    QHttpPart botPart;
    botPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"bot\""));
    botPart.setBody(bot() ? "1" : "0");
    multiPart->append(botPart);

    QHttpPart discoverablePart;
    discoverablePart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"discoverable\""));
    discoverablePart.setBody(discoverable() ? "1" : "0");
    multiPart->append(discoverablePart);

    QMimeDatabase mimeDatabase;

    if (backgroundUrl().isLocalFile()) {
        auto file = new QFile(backgroundUrl().toLocalFile());
        const auto mime = mimeDatabase.mimeTypeForUrl(backgroundUrl());
        if (file->open(QIODevice::ReadOnly)) {
            QHttpPart headerPart;
            headerPart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
            headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"header\""));
            headerPart.setBodyDevice(file);
            file->setParent(multiPart);
            multiPart->append(headerPart);
        }
    } else if (backgroundUrl().isEmpty()) {
        QHttpPart headerPart;
        headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"header\""));
        multiPart->append(headerPart);
    }

    if (avatarUrl().isLocalFile()) {
        auto file = new QFile(avatarUrl().toLocalFile());
        const auto mime = mimeDatabase.mimeTypeForUrl(avatarUrl());
        if (file->open(QIODevice::ReadOnly)) {
            QHttpPart headerPart;
            headerPart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
            headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"avatar\""));
            headerPart.setBodyDevice(file);
            file->setParent(multiPart);
            multiPart->append(headerPart);
        }
    } else if (avatarUrl().isEmpty()) {
        QHttpPart headerPart;
        headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"avatar\""));
        multiPart->append(headerPart);
    }

    m_account->patch(url, multiPart, true, this, [=](QNetworkReply *reply) {
        multiPart->setParent(reply);
        Q_EMIT sendNotification(i18n("Account details saved"));
        fetchAccountInfo();
    });
}

#include "moc_profileeditor.cpp"