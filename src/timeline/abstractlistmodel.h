// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include <QAbstractListModel>

class AbstractAccount;

/**
 *  @class AbstractListModel
 *  @brief The AbstractListModel class extends QAbstractListModel to provide a base class for custom list models.
 *  This class defines properties and methods that can be used by derived list models to implement custom functionality.
 */
class AbstractListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(bool shouldLoadMore READ shouldLoadMore WRITE setShouldLoadMore NOTIFY shouldLoadMoreChanged)

public:
    /**
     * @brief Constructs an AbstractListModel object.
     * @param parent The parent QObject.
     */
    explicit AbstractListModel(QObject *parent = nullptr);

    /**
     * @brief Returns the display name of the list model.
     * @return The display name as a QString.
     *
     * This method should be overridden by derived classes to provide the specific display name.
     */
    virtual QString displayName() const = 0;

    /**
     * @brief Returns the name of the list model.
     * @return The name as a QString.
     */
    QString name() const;

    /**
     * @brief Sets the name of the list model.
     * @param name The name to set.
     */
    void setName(const QString &name);

    /**
     * @brief Returns the loading state of the list model.
     * @return True if the list model is currently loading, false otherwise.
     */
    bool loading() const;
    /**
     * @brief Sets the loading state of the list model.
     * @param loading The loading state to set.
     */
    void setLoading(bool loading);
    /**
     * @brief Returns the account associated with the list model.
     * @return A pointer to the AbstractAccount object.
     */

    AbstractAccount *account() const;

    /**
     * @brief Returns whether the list model should load more items.
     * @return True if the list model should load more items, false otherwise.
     */
    bool shouldLoadMore() const;

    /**
     * @brief Sets whether the list model should load more items.
     * @param shouldLoadMore The value to set.
     */
    void setShouldLoadMore(bool shouldLoadMore);

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the name of the list model changes.
     */
    void nameChanged();
    /**
     * @brief This signal is emitted when the loading state of the list model changes.
     */
    void loadingChanged();
    /**
     * @brief This signal is emitted when the shouldLoadMore property changes.
     */
    void shouldLoadMoreChanged();

private:
    /**
     * @brief The name of the list model.
     */
    QString m_listName;
    /**
     * @brief The loading state of the list model.
     */
    bool m_loading = false;
    /**
     * @brief A pointer to the AbstractAccount object.
     */
    AbstractAccount *m_account = nullptr;
    /*
     * @brief A pointer to the AccountManager object.
     */
    AccountManager *m_manager = nullptr;
    /**
     * @brief Whether the list model should load more items.
     */
    bool m_shouldLoadMore = true;
};