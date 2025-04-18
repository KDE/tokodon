// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QQmlEngine>

#ifdef HAVE_KUNIFIEDPUSH
#include <kunifiedpush/connector.h>
#endif

/**
 * @brief Handles miscenallous operations, mostly a glue between C++ and QML.
 */
class NetworkController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Controller)
    QML_SINGLETON

    Q_PROPERTY(bool pushNotificationsAvailable READ pushNotificationsAvailable CONSTANT)
    Q_PROPERTY(QJsonArray errorMessages READ errorMessages NOTIFY errorMessagesChanged)

public:
    static NetworkController *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    static NetworkController &instance();

    /**
     * @brief Sets up the application proxy.
     * @param reloadAccounts Whether to tell AccountManager to reload the accounts for us.
     */
    Q_INVOKABLE void setApplicationProxy(bool reloadAccounts = true);

    /**
     * @brief Sets the authentication code for the current account.
     * @return A boolean if Tokodon accepted this as a valid auth token. This should fallback to alternative behavior if false.
     */
    bool setAuthCode(QUrl authCode);

    /**
     * @brief Opens the composer with @p text.
     * @note This is similiar to Navigation.openStatusComposer() except this is meant to be called before Tokodon fully loads. Otherwise it calls directly into
     * Navigation.
     */
    void startComposing(const QString &text);

    /**
     * @return Whether or not we have a push notification endpoint available.
     */
    [[nodiscard]] bool pushNotificationsAvailable() const;

    /**
     * @brief Opens an arbitrary HTML link (post, account, etc URL) if available on the current account's server.
     */
    Q_INVOKABLE void openLink(const QString &input);

    /**
     * @brief Same function as openLink, but for the web+ap protocol.
     */
    Q_INVOKABLE void openWebApLink(QString input);

    /**
     * @brief Log the error @p message which can be later viewed in the settings UI.
     *
     * This also prints this as a warning to the log.
     *
     * @note The number of error messages kept is limited to the 5 most recent.
     */
    void logError(const QString &url, const QString &message);

    /**
     * @return The last 5 most recent errors messages.
     */
    [[nodiscard]] QJsonArray errorMessages() const;

    /**
     * @brief Clear all of the error messages.
     */
    Q_INVOKABLE void clearErrorMessages();

    /**
     * @brief Register with the KUnifiedPush distributor, if available.
     */
    void setupPushNotifications();

#ifdef HAVE_KUNIFIEDPUSH
    /**
     * @return The KUnifiedPush connector.
     */
    KUnifiedPush::Connector *connector() const;
#endif

    QString endpoint;

Q_SIGNALS:
    /**
     * @brief Show a visible error message to the user.
     *
     * Should only be used in very limited scenarios that affect normal operation. A good example is timeline loading.
     */
    void networkErrorOccurred(const QString &errorString);
    void receivedAuthCode(QString authCode);
    void errorMessagesChanged();
    void pushMessageReceived(const QByteArray &msg);

private:
    explicit NetworkController(QObject *parent = nullptr);

    void openLink();

    QUrl m_requestedLink;
    bool m_accountsReady = false;
    QString m_storedComposedText;

    QByteArray m_systemHttpProxy;
    QByteArray m_systemHttpsProxy;

#ifdef HAVE_KUNIFIEDPUSH
    KUnifiedPush::Connector *m_connector = nullptr;
#endif
};
