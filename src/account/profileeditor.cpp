// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "profileeditor.h"
#include "abstractaccount.h"
#include <KLocalizedString>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QMimeDatabase>

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

bool ProfileEditorBackend::sensitive() const
{
    return m_sensitive;
}

void ProfileEditorBackend::setSensitive(bool sensitive)
{
    if (m_sensitive == sensitive) {
        return;
    }
    m_sensitive = sensitive;
    Q_EMIT sensitiveChanged();
}

QString ProfileEditorBackend::privacy() const
{
    return m_privacy;
}

void ProfileEditorBackend::setPrivacy(const QString &privacy)
{
    if (privacy == m_privacy) {
        return;
    }
    m_privacy = privacy;
    Q_EMIT privacyChanged();
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

QString ProfileEditorBackend::language() const
{
    return m_language;
}

void ProfileEditorBackend::setLanguage(const QString &language)
{
    if (language == m_language) {
        return;
    }
    m_language = language;
    Q_EMIT languageChanged();
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
        setDisplayName(obj["display_name"].toString());
        const auto source = obj["source"].toObject();
        setSensitive(source["sensitive"].toBool());
        setPrivacy(source["privacy"].toString());
        setNote(source["note"].toString());
        setLanguage(source["language"].toString());
        setBot(obj["bot"].toBool());
        setBackgroundUrl(QUrl(obj["header_static"].toString()));
        setAvatarUrl(QUrl(obj["avatar_static"].toString()));
        setLocked(obj["locked"].toBool());
        setDiscoverable(obj["discoverable"].toBool());
    });
}

void ProfileEditorBackend::save()
{
    const QUrl url = m_account->apiUrl("/api/v1/accounts/update_credentials");

    auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart displayNamePart;
    displayNamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"display_name\""));
    displayNamePart.setBody(displayName().toUtf8());
    multiPart->append(displayNamePart);

    QHttpPart notePart;
    notePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note\""));
    notePart.setBody(note().toUtf8());
    multiPart->append(notePart);

    QHttpPart lockedPart;
    lockedPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"locked\""));
    lockedPart.setBody(locked() ? "1" : "0");
    multiPart->append(lockedPart);

    QHttpPart botPart;
    botPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"bot\""));
    botPart.setBody(bot() ? "1" : "0");
    multiPart->append(botPart);

    QHttpPart discoverablePart;
    discoverablePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"discoverable\""));
    discoverablePart.setBody(discoverable() ? "1" : "0");
    multiPart->append(discoverablePart);

    QHttpPart privacyPart;
    privacyPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"source[privacy]\""));
    privacyPart.setBody(privacy().toUtf8());
    multiPart->append(privacyPart);

    QHttpPart sensitivityPart;
    sensitivityPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"source[sensitive]\""));
    sensitivityPart.setBody(sensitive() ? "1" : "0");
    multiPart->append(sensitivityPart);

    QMimeDatabase mimeDatabase;

    if (backgroundUrl().isLocalFile()) {
        auto file = new QFile(backgroundUrl().toLocalFile());
        const auto mime = mimeDatabase.mimeTypeForUrl(backgroundUrl());
        if (file->open(QIODevice::ReadOnly)) {
            QHttpPart headerPart;
            headerPart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
            headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"header\""));
            headerPart.setBodyDevice(file);
            file->setParent(multiPart);
            multiPart->append(headerPart);
        }
    } else if (backgroundUrl().isEmpty()) {
        QHttpPart headerPart;
        headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"header\""));
        multiPart->append(headerPart);
    }

    if (avatarUrl().isLocalFile()) {
        auto file = new QFile(avatarUrl().toLocalFile());
        const auto mime = mimeDatabase.mimeTypeForUrl(avatarUrl());
        if (file->open(QIODevice::ReadOnly)) {
            QHttpPart headerPart;
            headerPart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
            headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"avatar\""));
            headerPart.setBodyDevice(file);
            file->setParent(multiPart);
            multiPart->append(headerPart);
        }
    } else if (avatarUrl().isEmpty()) {
        QHttpPart headerPart;
        headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"avatar\""));
        multiPart->append(headerPart);
    }

    m_account->patch(url, multiPart, true, this, [=](QNetworkReply *reply) {
        multiPart->setParent(reply);
        Q_EMIT sendNotification(i18n("Account details saved"));
        fetchAccountInfo();
    });
}
