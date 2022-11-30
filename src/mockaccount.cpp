#include "mockaccount.h"

MockAccount::MockAccount(QObject *parent)
    : AbstractAccount(parent)
{}

void MockAccount::get(const QUrl &url, bool authenticated, std::function<void(QNetworkReply *)> callback) 
{
}
void MockAccount::post(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)> callback) 
{}
void MockAccount::post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, std::function<void(QNetworkReply *)> callback) 
{}
void MockAccount::post(const QUrl &url, QHttpMultiPart *message, bool authenticated, std::function<void(QNetworkReply *)> callback) 
{}
void MockAccount::put(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)> callback) 
{}
void MockAccount::upload(Post *post, QFile *file, const QString &filename) 
{}

void MockAccount::writeToSettings(QSettings &settings) const
{}
void MockAccount::buildFromSettings(const QSettings &settings)
{}

void MockAccount::validateToken()
{}
