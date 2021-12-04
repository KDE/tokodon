// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "account.h"
#include "accountmanager.h"
#include <QDebug>
#include <QUrlQuery>

#if HAVE_KACCOUNTS
#include <GetCredentialsJob>
#endif

Account::Account(const QString &name, const QString &instance_uri, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_qnam(new QNetworkAccessManager(this))
    // default to 500, instances which support more signal it
    , m_maxPostLength(500)
{
    setInstanceUri(instance_uri);
    m_identity.reparentIdentity(this);
}

Account::Account(const QSettings &settings, QObject *parent)
    : QObject(parent)
    , m_qnam(new QNetworkAccessManager(this))
    , m_maxPostLength(500)
{
    m_identity.reparentIdentity(this);

    buildFromSettings(settings);
}

#if HAVE_KACCOUNTS
Account::Account(const QString &name, Accounts::Account *account, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_qnam(new QNetworkAccessManager(this))
    , m_maxPostLength(500)
    , m_kaccountsAccount(account)
{
    m_identity.reparentIdentity(this);

    auto job = new GetCredentialsJob(account->id(), this);

    connect(job, &GetCredentialsJob::finished, this, [job, this] {
        const QVariantMap data = job->credentialsData();
        m_token = data.value("AccessToken").toString();
        m_client_id = data.value("ClientId").toString();
        m_client_secret = data.value("ClientSecret").toString();
        m_instance_uri = QStringLiteral("https://") + data.value("Host").toString();
    });

    job->start();
}
#endif

Account::~Account()
{
    m_identityCache.clear();
}

QUrl Account::apiUrl(const QString &path) const
{
    auto url = QUrl::fromUserInput(m_instance_uri);
    url.setScheme("https");
    url.setPath(path);

    return url;
}

void Account::registerApplication()
{
    // clear any previous bearer token credentials
    m_token = QString();

    // register
    const QUrl regUrl = apiUrl("/api/v1/apps");
    QJsonObject obj;

    obj["client_name"] = "Tokodon";
    obj["redirect_uris"] = "urn:ietf:wg:oauth:2.0:oob";
    obj["scopes"] = "read write follow";
    obj["website"] = "https://invent.kde.org/carlschwan/kmasto";

    const QJsonDocument doc(obj);

    post(regUrl, doc, false, [=](QNetworkReply *reply) {
        if (!reply->isFinished())
            return;

        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        m_client_id = doc.object()["client_id"].toString();
        m_client_secret = doc.object()["client_secret"].toString();

        if (isRegistered())
            Q_EMIT registered();
    });
}

void Account::setDirtyIdentity()
{
    Q_EMIT identityChanged(this);
}

bool Account::isRegistered() const
{
    return !m_client_id.isEmpty() && !m_client_secret.isEmpty();
}

void Account::get(const QUrl &url, bool authenticated, std::function<void(QNetworkReply *)> reply_cb)
{
    QNetworkRequest request = QNetworkRequest(url);

    if (authenticated && haveToken()) {
        const QByteArray bearer = QString("Bearer " + m_token).toLocal8Bit();
        request.setRawHeader("Authorization", bearer);
    }

    QNetworkReply *reply = m_qnam->get(request);

    if (reply_cb != nullptr) {
        QObject::connect(reply, &QNetworkReply::finished, [reply, reply_cb, url]() {
            if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute)) {
                qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) << url;
                return;
            }

            reply_cb(reply);
        });
    }
}

void Account::post(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)> reply_cb)
{
    auto post_data = doc.toJson();

    QNetworkRequest request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (authenticated && haveToken()) {
        QByteArray bearer = QString("Bearer " + m_token).toLocal8Bit();
        request.setRawHeader("Authorization", bearer);
    }

    qDebug() << "POST" << url << "[" << post_data << "]";

    QNetworkReply *reply = m_qnam->post(request, post_data);

    if (reply_cb != nullptr) {
        QObject::connect(reply, &QNetworkReply::finished, [reply, reply_cb]() {
            if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
                return;

            reply_cb(reply);
        });
    }
}

void Account::put(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)> reply_cb)
{
    auto post_data = doc.toJson();

    QNetworkRequest request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (authenticated && haveToken()) {
        QByteArray bearer = QString("Bearer " + m_token).toLocal8Bit();
        request.setRawHeader("Authorization", bearer);
    }

    qDebug() << "PUT" << url << "[" << post_data << "]";

    QNetworkReply *reply = m_qnam->put(request, post_data);

    if (reply_cb != nullptr) {
        QObject::connect(reply, &QNetworkReply::finished, [reply, reply_cb]() {
            if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
                return;

            reply_cb(reply);
        });
    }
}

void Account::post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, std::function<void(QNetworkReply *)> reply_cb)
{
    auto post_data = formdata.toString().toLatin1();

    QNetworkRequest request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    if (authenticated && haveToken()) {
        QByteArray bearer = QString("Bearer " + m_token).toLocal8Bit();
        request.setRawHeader("Authorization", bearer);
    }

    qDebug() << "POST" << url << "[" << post_data << "]";

    QNetworkReply *reply = m_qnam->post(request, post_data);

    if (reply_cb != nullptr) {
        QObject::connect(reply, &QNetworkReply::finished, [reply, reply_cb]() {
            if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
                return;

            reply_cb(reply);
        });
    }
}

void Account::post(const QUrl &url, QHttpMultiPart *message, bool authenticated, std::function<void(QNetworkReply *)> reply_cb)
{
    QNetworkRequest request = QNetworkRequest(url);

    qDebug() << "POST" << url << "(multipart-message)";

    if (authenticated && haveToken()) {
        QByteArray bearer = QString("Bearer " + m_token).toLocal8Bit();
        request.setRawHeader("Authorization", bearer);
    }

    QNetworkReply *reply = m_qnam->post(request, message);
    message->setParent(reply);

    if (reply_cb != nullptr) {
        QObject::connect(reply, &QNetworkReply::finished, [reply, reply_cb]() {
            if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
                return;

            reply_cb(reply);
        });
    }
}

// assumes file is already opened and named
void Account::upload(Post *p, QFile *file, QString filename)
{
    QHttpMultiPart *mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("attachment; name=\"file\"; filename=\"%1\"").arg(filename));
    filePart.setBodyDevice(file);
    file->setParent(mp);

    mp->append(filePart);

    auto upload_url = apiUrl("/api/v1/media");
    post(upload_url, mp, true, [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (doc.isObject())
            return;

        auto obj = doc.object();

        auto att = new Attachment(p, obj);
        if (att->m_url.isEmpty())
            return;

        p->m_attachments.append(att);

        Q_EMIT attachmentUploaded(p, att);
    });
}

// best effort for now, maybe we fire a signal later
void Account::updateAttachment(Attachment *a)
{
    const auto att_url = apiUrl(QString("/api/v1/media/%1").arg(a->m_id));
    QJsonObject obj;

    obj["description"] = a->m_description;

    const QJsonDocument doc(obj);

    put(att_url, doc, true, nullptr);
}

const std::shared_ptr<Identity> Account::identityLookup(const QString &acct, const QJsonObject &doc)
{
    auto id = m_identityCache[acct];
    if (id && id->m_acct == acct)
        return m_identityCache[acct];

    id = std::make_shared<Identity>();
    id->reparentIdentity(this);
    id->fromSourceData(doc);
    m_identityCache[acct] = id;

    return m_identityCache[acct];
}

bool Account::identityCached(const QString &acct) const
{
    auto id = m_identityCache[acct];
    return id && id->m_acct == acct;
}

QUrlQuery Account::buildOAuthQuery() const
{
    QUrlQuery q = QUrlQuery();

    q.addQueryItem("client_id", m_client_id);

    return q;
}

QUrl Account::getAuthorizeUrl() const
{
    QUrl url = apiUrl("/oauth/authorize");
    QUrlQuery q = buildOAuthQuery();

    q.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
    q.addQueryItem("response_type", "code");
    q.addQueryItem("scope", "read write follow");

    url.setQuery(q);

    return url;
}

QUrl Account::getTokenUrl() const
{
    return apiUrl("/oauth/token");
}

void Account::setInstanceUri(const QString &instance_uri)
{
    if (m_instance_uri == instance_uri)
        return;

    // instance URI changed, get new credentials
    QUrl instance_url = QUrl::fromUserInput(instance_uri);
    instance_url.setScheme("https"); // getting token from http is not supported

    m_instance_uri = instance_url.toString();
    registerApplication();
}

QString Account::instanceUri() const
{
    return QString(m_instance_uri);
}

void Account::setToken(const QString &authcode)
{
    QUrl tokenUrl = getTokenUrl();
    QUrlQuery q = buildOAuthQuery();

    q.addQueryItem("client_secret", m_client_secret);
    q.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
    q.addQueryItem("grant_type", "authorization_code");
    q.addQueryItem("code", authcode);

    post(tokenUrl, q, false, [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        m_token = doc.object()["access_token"].toString();
        AccountManager::instance().addAccount(this);
        validateToken();
    });
}

void Account::validateToken()
{
    if (!haveToken()) {
        return;
    }

    const QUrl verify_credentials = apiUrl("/api/v1/accounts/verify_credentials");

    get(verify_credentials, true, [=](QNetworkReply *reply) {
        if (!reply->isFinished()) {
            return;
        }

        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        if (!doc.isObject() || !doc.object().contains("source"))
            return;

        Q_EMIT authenticated();

        m_identity.fromSourceData(doc.object());
    });

    fetchInstanceMetadata();

    // set up streaming for notifications
    streamingSocket("user");
}

void Account::writeToSettings(QSettings &settings) const
{
#if HAVE_KACCOUNTS
    if (m_kaccountsAccount) {
        // Don't save KAccounts accounts to settings
        return;
    }
#endif

    settings.beginGroup(m_name);

    settings.setValue("token", m_token);
    settings.setValue("client_id", m_client_id);
    settings.setValue("client_secret", m_client_secret);
    settings.setValue("instance_uri", m_instance_uri);
    settings.setValue("name", m_name);

    settings.endGroup();
}

void Account::buildFromSettings(const QSettings &settings)
{
    if (!settings.value("token").toString().isEmpty()) {
        m_token = settings.value("token").toString();
        m_client_id = settings.value("client_id").toString();
        m_client_secret = settings.value("client_secret").toString();
        m_name = settings.value("name").toString();
        m_instance_uri = settings.value("instance_uri").toString();
        validateToken();
    }
}

void Account::fetchAccount(int id, bool excludeReplies, std::function<void(QList<std::shared_ptr<Post>>)> final_cb)
{
    QList<std::shared_ptr<Post>> *thread = new QList<std::shared_ptr<Post>>;

    QUrl uriStatus(m_instance_uri);
    uriStatus.setPath(QString("/api/v1/accounts/%1/statuses").arg(id));
    QUrlQuery q;
    if (excludeReplies) {
        q.addQueryItem("exclude_replies", "true");
    }
    uriStatus.setQuery(q);

    QUrl uriPinned(m_instance_uri);
    QUrlQuery q1;
    uriPinned.setPath(QString("/api/v1/accounts/%1/statuses").arg(id));
    q1.addQueryItem("pinned", "true");
    uriPinned.setQuery(q1);

    auto onFetchPinned = [=](QNetworkReply *reply) {
        QList<std::shared_ptr<Post>> posts;
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            return;
        }
        int i = 0;
        for (const auto &value : doc.array()) {
            const QJsonObject obj = value.toObject();

            auto p = std::make_shared<Post>(this, obj);
            thread->insert(i, p);
            i++;
        }

        QList<std::shared_ptr<Post>> finalThread = QList<std::shared_ptr<Post>>(*thread);
        delete thread;
        final_cb(finalThread);
    };

    auto onFetchAccount = [=](QNetworkReply *reply) {
        QList<std::shared_ptr<Post>> posts;

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            return;
        }

        for (const auto &value : doc.array()) {
            const QJsonObject obj = value.toObject();

            auto p = std::make_shared<Post>(this, obj);
            thread->push_back(p);
        }

        get(uriPinned, true, onFetchPinned);
    };

    get(uriStatus, true, onFetchAccount);
}

void Account::fetchTimeline(const QString &original_name, const QString &from_id)
{
    QString timelineName = QString(original_name);
    bool local = timelineName == "public";

    // federated timeline is really "public" without local set
    if (timelineName == "federated") {
        timelineName = "public";
    }

    QUrlQuery q;
    if (local) {
        q.addQueryItem("local", "true");
    }
    if (!from_id.isEmpty()) {
        q.addQueryItem("max_id", from_id);
    }
    QUrl uri = QUrl::fromUserInput(m_instance_uri);
    uri.setScheme("https");
    uri.setPath(QString("/api/v1/timelines/%1").arg(timelineName));
    uri.setQuery(q);

    get(uri, true, [=](QNetworkReply *reply) {
        QList<std::shared_ptr<Post>> posts;

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            return;
        }

        for (const auto &value : doc.array()) {
            QJsonObject obj = value.toObject();

            const auto p = std::make_shared<Post>(this, obj);
            posts.push_back(p);
        }

        Q_EMIT fetchedTimeline(original_name, posts);
    });
}

void Account::fetchThread(const QString &post_id, std::function<void(QList<std::shared_ptr<Post>>)> final_cb)
{
    auto status_url = apiUrl(QString("/api/v1/statuses/%1").arg(post_id));
    auto context_url = apiUrl(QString("/api/v1/statuses/%1/context").arg(post_id));
    QList<std::shared_ptr<Post>> *thread = new QList<std::shared_ptr<Post>>;

    auto on_fetch_context = [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();

        if (!doc.isObject())
            return;

        const auto ancestors = obj["ancestors"].toArray();

        for (const auto &anc : ancestors) {
            if (!anc.isObject())
                continue;

            auto anc_obj = anc.toObject();
            auto p = std::make_shared<Post>(this, anc_obj);

            thread->push_front(p);
        }

        const auto descendents = obj["descendants"].toArray();

        for (const auto &desc : descendents) {
            if (!desc.isObject())
                continue;

            auto desc_obj = desc.toObject();
            auto p = std::make_shared<Post>(this, desc_obj);

            thread->push_back(p);
        }

        QList<std::shared_ptr<Post>> finalThread = QList<std::shared_ptr<Post>>(*thread);

        delete thread;
        final_cb(finalThread);
    };

    auto on_fetch_status = [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();

        if (!doc.isObject())
            return;

        auto p = std::make_shared<Post>(this, obj);
        thread->push_front(p);

        get(context_url, true, on_fetch_context);
    };

    get(status_url, true, on_fetch_status);
}

void Account::postStatus(Post *p)
{
    QUrl post_status_url = apiUrl("/api/v1/statuses");
    auto doc = p->toJsonDocument();

    post(post_status_url, doc, true, [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        qDebug() << "Message sent:" << obj;
    });
}

void Account::mutatePost(std::shared_ptr<Post> p, const QString &verb, bool deliver_home)
{
    QUrl mutation_url = apiUrl(QString("/api/v1/statuses/%1/%2").arg(p->inReplyTo(), verb));
    QJsonDocument doc;

    post(mutation_url, doc, true, [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        auto post_id = doc.object()["id"].toString();

        if (deliver_home) {
            QList<std::shared_ptr<Post>> posts;
            auto obj = doc.object();

            auto p = std::make_shared<Post>(this, obj);
            posts.push_back(p);

            Q_EMIT fetchedTimeline("home", posts);
        }
    });
}

void Account::favorite(std::shared_ptr<Post> p)
{
    mutatePost(p, "favourite");
}

void Account::unfavorite(std::shared_ptr<Post> p)
{
    mutatePost(p, "unfavourite");
}

void Account::repeat(std::shared_ptr<Post> p)
{
    mutatePost(p, "reblog", true);
}

void Account::unrepeat(std::shared_ptr<Post> p)
{
    mutatePost(p, "unreblog");
}

// It seemed clearer to keep this logic separate from the general instance metadata collection, on the off chance
// that it might need to be extended later on.
static Account::AllowedContentType parseVersion(const QString &instanceVer)
{
    using ContentType = Account::AllowedContentType;

    unsigned int result = ContentType::PlainText;
    if (instanceVer.contains("glitch"))
        result |= ContentType::Markdown | ContentType::Html;

    return static_cast<ContentType>(result);
}

static QMap<QString, Account::AllowedContentType> str_to_content_type = {{"text/plain", Account::AllowedContentType::PlainText},
                                                                         {"text/bbcode", Account::AllowedContentType::BBCode},
                                                                         {"text/html", Account::AllowedContentType::Html},
                                                                         {"text/markdown", Account::AllowedContentType::Markdown}};

static Account::AllowedContentType parse_pleroma_info(const QJsonDocument &doc)
{
    using ContentType = Account::AllowedContentType;
    unsigned int result = ContentType::PlainText;

    auto obj = doc.object();
    if (obj.contains("metadata")) {
        auto metadata = obj["metadata"].toObject();
        if (!metadata.contains("postFormats"))
            return static_cast<ContentType>(result);

        auto formats = metadata["postFormats"].toArray();

        for (auto c : formats) {
            auto fmt = c.toString();

            if (!str_to_content_type.contains(fmt))
                continue;

            result |= (unsigned int)str_to_content_type[fmt];
        }
    }

    return static_cast<ContentType>(result);
}

void Account::fetchInstanceMetadata()
{
    QUrl instance_url = apiUrl("/api/v1/instance");
    QUrl pleroma_info = apiUrl("/nodeinfo/2.1.json");

    get(instance_url, false, [=](QNetworkReply *reply) {
        if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
            return;

        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        if (!doc.isObject())
            return;

        auto obj = doc.object();

        if (obj.contains("max_toot_chars"))
            m_maxPostLength = (unsigned)obj["max_toot_chars"].toInt();

        // One can only hope that there will always be a version attached
        if (obj.contains("version"))
            m_allowedContentTypes = parseVersion(obj["version"].toString());

        m_instance_name = obj["title"].toString();
        Q_EMIT fetchedInstanceMetadata();
    });

    get(pleroma_info, false, [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        m_allowedContentTypes = parse_pleroma_info(doc);
        Q_EMIT fetchedInstanceMetadata();
    });
}

void Account::invalidate()
{
    Q_EMIT invalidated();
}

void Account::invalidatePost(Post *p)
{
    Q_EMIT invalidatedPost(p);
}

QUrl Account::streamingUrl(const QString &stream)
{
    QUrl url = apiUrl("/api/v1/streaming");
    QUrlQuery q;

    q.addQueryItem("access_token", m_token);
    q.addQueryItem("stream", stream);

    url.setQuery(q);
    url.setScheme("wss");

    return QUrl(url);
}

QWebSocket *Account::streamingSocket(const QString &stream)
{
    if (m_websockets.contains(stream))
        return m_websockets[stream];

    auto socket = new QWebSocket();
    socket->setParent(this);

    auto url = streamingUrl(stream);

    QObject::connect(socket, &QWebSocket::textMessageReceived, [=](const QString &message) {
        QString target_tl = stream;
        auto env = QJsonDocument::fromJson(message.toLocal8Bit());

        if (stream == "user")
            target_tl = "home";

        auto event = env.object()["event"].toString();
        if (event == "update") {
            QSettings settings;
            QJsonDocument doc = QJsonDocument::fromJson(env.object()["payload"].toString().toLocal8Bit());
            auto account_obj = doc.object()["account"].toObject();

            if (account_obj["acct"] == m_identity.m_acct)
                return;

            if (settings.value("Preferences/timeline_firehose", true).toBool())
                handleUpdate(doc, target_tl);

            return;
        } else if (event == "notification") {
            QJsonDocument doc = QJsonDocument::fromJson(env.object()["payload"].toString().toLocal8Bit());

            handleNotification(doc);
            return;
        }
    });

    socket->open(url);

    qDebug() << "[WEBSOCKET] Connecting to" << url;

    m_websockets[stream] = socket;
    return socket;
}

void Account::handleUpdate(const QJsonDocument &doc, const QString &target)
{
    QList<std::shared_ptr<Post>> posts;

    auto obj = doc.object();

    auto p = std::make_shared<Post>(this, obj);
    posts.push_back(p);

    Q_EMIT fetchedTimeline(target, posts);
}

void Account::handleNotification(const QJsonDocument &doc)
{
    auto obj = doc.object();
    std::shared_ptr<Notification> n = std::make_shared<Notification>(this, obj);

    Q_EMIT notification(n);
}

Post *Account::newPost()
{
    return new Post(this);
}

void Identity::fromSourceData(const QJsonObject &doc)
{
    m_id = doc["id"].toString().toInt();
    m_display_name = doc["display_name"].toString();
    m_acct = doc["acct"].toString();
    m_bio = doc["note"].toString();
    m_locked = doc["locked"].toBool();
    m_backgroundUrl = QUrl(doc["header"].toString());
    m_followersCount = doc["followers_count"].toInt();
    m_followingCount = doc["following_count"].toInt();
    m_statusesCount = doc["statuses_count"].toInt();
    m_fields = doc["fields"].toArray();

    // When the user data is ourselves, we get source.privacy
    // with the default post privacy setting for the user. all others
    // will get empty strings.
    QJsonObject source = doc["source"].toObject();
    m_visibility = source["privacy"].toString();

    m_avatarUrl = QUrl(doc["avatar"].toString());

    if (m_acct == m_parent->identity().m_acct)
        m_parent->setDirtyIdentity();
}

void Identity::reparentIdentity(Account *parent)
{
    m_parent = parent;
}
