// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "account/profileeditor.h"

#include "account/abstractaccount.h"
#include "utils/texthandler.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QTextDocument>
#include <qfileinfo.h>
#include <qhttpmultipart.h>
#include <qmimedatabase.h>

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
    if (m_account != nullptr) {
        if (m_displayName.isEmpty()) {
            return m_account->username();
        }
        return TextHandler::replaceCustomEmojis(m_account->customEmojis(), displayName());
    } else {
        return displayName();
    }
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

QJsonArray ProfileEditorBackend::fields() const
{
    return m_fields;
}

void ProfileEditorBackend::setFields(const QJsonArray &fields)
{
    if (m_fields == fields) {
        return;
    }
    m_fields = fields;
    Q_EMIT fieldsChanged();
}

void ProfileEditorBackend::setFieldName(const int index, const QString &name)
{
    auto field = m_fields[index].toObject();
    field["name"_L1] = name;
    m_fields[index] = field;
}

void ProfileEditorBackend::setFieldValue(const int index, const QString &value)
{
    auto field = m_fields[index].toObject();
    field["value"_L1] = value;
    m_fields[index] = field;
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
        // to handle cases where display name is empty, and the signal would never be sent otherwise
        if (m_displayName.isEmpty()) {
            Q_EMIT displayNameChanged();
        }
        const auto source = obj["source"_L1].toObject();
        setNote(source["note"_L1].toString());
        setBot(obj["bot"_L1].toBool());
        setBackgroundUrl(QUrl(obj["header_static"_L1].toString()));
        setAvatarUrl(QUrl(obj["avatar_static"_L1].toString()));
        setLocked(obj["locked"_L1].toBool());
        setDiscoverable(obj["discoverable"_L1].toBool());

        m_fields = {};
        // The field value is HTML, we must process and strip it
        for (const auto &fieldVal : obj["fields"_L1].toArray()) {
            auto field = fieldVal.toObject();
            QTextDocument document;
            document.setHtml(field["value"_L1].toString());
            field["value"_L1] = document.toPlainText();

            m_fields.append(field);
        }
        Q_EMIT fieldsChanged();

        m_account->identity()->fromSourceData(obj);
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

    for (int i = 0; i < maxFields(); i++) {
        QHttpPart fieldNamePart;
        fieldNamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"fields_attributes[%1][name]\"").arg(i));
        fieldNamePart.setBody(m_fields[i]["name"_L1].toString().toUtf8());
        multiPart->append(fieldNamePart);

        QHttpPart fieldValuePart;
        fieldValuePart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"fields_attributes[%1][value]\"").arg(i));
        fieldValuePart.setBody(m_fields[i]["value"_L1].toString().toUtf8());
        multiPart->append(fieldValuePart);
    }

    QMimeDatabase mimeDatabase;

    if (backgroundUrl().isLocalFile()) {
        auto file = new QFile(backgroundUrl().toLocalFile());
        const auto mime = mimeDatabase.mimeTypeForUrl(backgroundUrl());
        if (file->open(QIODevice::ReadOnly)) {
            QHttpPart headerPart;
            headerPart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
            headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"header\"; filename=\"header\""));
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
            headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"avatar\"; filename=\"avatar\""));
            headerPart.setBodyDevice(file);
            file->setParent(multiPart);
            multiPart->append(headerPart);
        }
    } else if (avatarUrl().isEmpty()) {
        QHttpPart headerPart;
        headerPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"avatar\""));
        multiPart->append(headerPart);
    }

    m_account->patch(url, multiPart, true, this, [this, multiPart](QNetworkReply *reply) {
        multiPart->setParent(reply);
        Q_EMIT sendNotification(i18n("Account details saved"));
        fetchAccountInfo();
    });
}

int ProfileEditorBackend::maxFields() const
{
    // TODO: some instances support more fields
    return 4;
}

void ProfileEditorBackend::addField()
{
    m_fields.push_back({});
    Q_EMIT fieldsChanged();
}

void ProfileEditorBackend::removeField()
{
    m_fields.pop_back();
    Q_EMIT fieldsChanged();
}

#include "moc_profileeditor.cpp"
