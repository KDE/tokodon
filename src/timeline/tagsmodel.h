// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QtQml/qqmlregistration.h>

#include "abstractlistmodel.h"
#include "tag.h"

/**
 * @class TagsModel
 * @brief The TagsModel class provides a model for displaying a list of tags.
 * The TagsModel class is a subclass of AbstractListModel and provides
 * functionality for displaying a list of tags. It inherits the basic model
 * functionality from AbstractListModel and extends it to handle tags.
 */
class TagsModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum CustomRoles { NameRole = Qt::UserRole + 1, UrlRole, HistoryRole };

    /**
     * @brief Constructs a TagsModel object.
     * @param parent The parent object (optional).
     */
    explicit TagsModel(QObject *parent = nullptr);

    /**
     * @brief Returns the number of rows in the model.
     * @param parent The parent index (unused).
     * @return The number of rows in the model.
     */
    int rowCount(const QModelIndex &parent) const override;

    /**
     * @brief Returns the data for the given role and index.
     * @param index The index of the item.
     * @param role The role for which to return data.
     * @return The data for the given role and index.
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief Returns the role names used by the model.
     * @return The role names used by the model.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Fills the timeline with tags starting from the specified ID.
     * @param fromId The ID from which to start filling the timeline (optional).
     */
    void fillTimeline(const QString &fromId = {});

    /**
     * @brief Fetches more data for the specified parent index.
     * @param parent The parent index.
     */
    void fetchMore(const QModelIndex &parent) override;

    /**
     * @brief Checks if more data can be fetched for the specified parent index.
     * @param parent The parent index.
     * @return True if more data can be fetched, false otherwise.
     */
    bool canFetchMore(const QModelIndex &parent) const override;
    /**
     * @brief Returns the display name of the model.
     * @return The display name of the model.
     */
    QString displayName() const override;

public Q_SLOTS:
    /**
     * @brief Slot called when the name is changed.
     */
    void onNameChanged();

private:
    /**
     * @brief The next URL for fetching more data.
     */
    QUrl m_next;
    /**
     * @brief The list of tags in the model.
     */
    QList<Tag> m_tags;
};
