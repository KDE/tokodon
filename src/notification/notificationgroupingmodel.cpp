// SPDX-FileCopyrightText: 2016 Eike Hein <hein@kde.org>
// SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "notificationgroupingmodel.h"

#include "notificationmodel.h"

NotificationGroupingModel::NotificationGroupingModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

bool NotificationGroupingModel::notificationsMatch(const QModelIndex &a, const QModelIndex &b)
{
    const QString aId = a.data(AbstractTimelineModel::CustoRoles::IdRole).toString();
    const QString bId = b.data(AbstractTimelineModel::CustoRoles::IdRole).toString();

    const QVariant aType = a.data(AbstractTimelineModel::CustoRoles::TypeRole);
    const QVariant bType = b.data(AbstractTimelineModel::CustoRoles::TypeRole);

    // it makes no sense to group poll or edit updates
    if (aType == bType && (aType == Notification::Type::Poll || aType == Notification::Type::Update || aType == Notification::Type::Status)) {
        return false;
    }

    return aId == bId && aType == bType;
}

bool NotificationGroupingModel::isGroup(int row) const
{
    if (row < 0 || row >= rowMap.count()) {
        return false;
    }

    return (rowMap.at(row)->count() > 1);
}

bool NotificationGroupingModel::tryToGroup(const QModelIndex &sourceIndex, bool silent)
{
    // Meat of the matter: Try to add this source row to a sub-list with source rows
    // associated with the same application.
    for (int i = 0; i < rowMap.count(); ++i) {
        const QModelIndex &groupRep = sourceModel()->index(rowMap.at(i)->constFirst(), 0);

        // Don't match a row with itself.
        if (sourceIndex == groupRep) {
            continue;
        }

        if (notificationsMatch(sourceIndex, groupRep)) {
            const QModelIndex parent = index(i, 0, QModelIndex());

            if (!silent) {
                const int newIndex = rowMap.at(i)->count();

                if (newIndex == 1) {
                    beginInsertRows(parent, 0, 1);
                } else {
                    beginInsertRows(parent, newIndex, newIndex);
                }
            }

            rowMap[i]->append(sourceIndex.row());

            if (!silent) {
                endInsertRows();

                Q_EMIT dataChanged(parent, parent);
            }

            return true;
        }
    }

    return false;
}

void NotificationGroupingModel::adjustMap(int anchor, int delta)
{
    for (int i = 0; i < rowMap.count(); ++i) {
        QVector<int> *sourceRows = rowMap.at(i);
        for (auto it = sourceRows->begin(); it != sourceRows->end(); ++it) {
            if ((*it) >= anchor) {
                *it += delta;
            }
        }
    }
}

void NotificationGroupingModel::rebuildMap()
{
    qDeleteAll(rowMap);
    rowMap.clear();

    const int rows = sourceModel()->rowCount();

    rowMap.reserve(rows);

    for (int i = 0; i < rows; ++i) {
        rowMap.append(new QVector<int>{i});
    }

    checkGrouping(true);
}

void NotificationGroupingModel::checkGrouping(bool silent)
{
    for (int i = (rowMap.count()) - 1; i >= 0; --i) {
        if (isGroup(i)) {
            continue;
        }

        if (tryToGroup(sourceModel()->index(rowMap.at(i)->constFirst(), 0), silent)) {
            beginRemoveRows(QModelIndex(), i, i);
            delete rowMap.takeAt(i);
            endRemoveRows();
        }
    }
}

void NotificationGroupingModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (sourceModel == QAbstractProxyModel::sourceModel()) {
        return;
    }

    beginResetModel();

    if (QAbstractProxyModel::sourceModel()) {
        QAbstractProxyModel::sourceModel()->disconnect(this);
    }

    QAbstractProxyModel::setSourceModel(sourceModel);

    if (sourceModel) {
        rebuildMap();

        connect(sourceModel, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &parent, int start, int end) {
            if (parent.isValid()) {
                return;
            }

            adjustMap(start, (end - start) + 1);

            for (int i = start; i <= end; ++i) {
                if (!tryToGroup(this->sourceModel()->index(i, 0))) {
                    beginInsertRows(QModelIndex(), rowMap.count(), rowMap.count());
                    rowMap.append(new QVector<int>{i});
                    endInsertRows();
                }
            }

            checkGrouping();
        });

        connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex &parent, int first, int last) {
            if (parent.isValid()) {
                return;
            }

            for (int i = first; i <= last; ++i) {
                for (int j = 0; j < rowMap.count(); ++j) {
                    const QVector<int> *sourceRows = rowMap.at(j);
                    const int mapIndex = sourceRows->indexOf(i);

                    if (mapIndex != -1) {
                        // Remove top-level item.
                        if (sourceRows->count() == 1) {
                            beginRemoveRows(QModelIndex(), j, j);
                            delete rowMap.takeAt(j);
                            endRemoveRows();
                            // Dissolve group.
                        } else if (sourceRows->count() == 2) {
                            const QModelIndex parent = index(j, 0, QModelIndex());
                            beginRemoveRows(parent, 0, 1);
                            rowMap[j]->remove(mapIndex);
                            endRemoveRows();

                            // We're no longer a group parent.
                            Q_EMIT dataChanged(parent, parent);
                            // Remove group member.
                        } else {
                            const QModelIndex parent = index(j, 0, QModelIndex());
                            beginRemoveRows(parent, mapIndex, mapIndex);
                            rowMap[j]->remove(mapIndex);
                            endRemoveRows();

                            // Various roles of the parent evaluate child data, and the
                            // child list has changed.
                            Q_EMIT dataChanged(parent, parent);

                            // Signal children count change for all other items in the group.
                            Q_EMIT dataChanged(index(0, 0, parent), index(rowMap.count() - 1, 0, parent), {AbstractTimelineModel::NumInGroupRole});
                        }

                        break;
                    }
                }
            }
        });

        connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, [this](const QModelIndex &parent, int start, int end) {
            if (parent.isValid()) {
                return;
            }

            adjustMap(start + 1, -((end - start) + 1));

            checkGrouping();
        });

        connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &NotificationGroupingModel::beginResetModel);
        connect(sourceModel, &QAbstractItemModel::modelReset, this, [this] {
            rebuildMap();
            endResetModel();
        });

        connect(sourceModel,
                &QAbstractItemModel::dataChanged,
                this,
                [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
                    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
                        const QModelIndex &sourceIndex = this->sourceModel()->index(i, 0);
                        QModelIndex proxyIndex = mapFromSource(sourceIndex);

                        if (!proxyIndex.isValid()) {
                            return;
                        }

                        const QModelIndex parent = proxyIndex.parent();

                        // If a child item changes, its parent may need an update as well as many of
                        // the data roles evaluate child data. See data().
                        // TODO: Some roles do not need to bubble up as they fall through to the first
                        // child in data(); it _might_ be worth adding constraints here later.
                        if (parent.isValid()) {
                            Q_EMIT dataChanged(parent, parent, roles);
                        }

                        Q_EMIT dataChanged(proxyIndex, proxyIndex, roles);
                    }
                });
    }

    endResetModel();
}

int NotificationGroupingModel::rowCount(const QModelIndex &parent) const
{
    if (!sourceModel()) {
        return 0;
    }

    if (parent.isValid() && parent.model() == this) {
        // Don't return row count for top-level item at child row: Group members
        // never have further children of their own.
        if (parent.parent().isValid()) {
            return 0;
        }

        if (parent.row() < 0 || parent.row() >= rowMap.count()) {
            return 0;
        }

        const int rowCount = rowMap.at(parent.row())->count();
        // If this sub-list in the map only has one entry, it's a plain item, not
        // parent to a group.
        if (rowCount == 1) {
            return 0;
        } else {
            return rowCount;
        }
    }

    return rowMap.count();
}

bool NotificationGroupingModel::hasChildren(const QModelIndex &parent) const
{
    if ((parent.model() && parent.model() != this) || !sourceModel()) {
        return false;
    }

    return rowCount(parent);
}

int NotificationGroupingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant NotificationGroupingModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || proxyIndex.model() != this || !sourceModel()) {
        return {};
    }

    const QModelIndex &parent = proxyIndex.parent();
    const bool isGroup = (!parent.isValid() && this->isGroup(proxyIndex.row()));

    // For group parent items, this will map to the last child task.
    const QModelIndex &sourceIndex = mapToSource(proxyIndex);

    if (!sourceIndex.isValid()) {
        return {};
    }

    if (isGroup) {
        switch (role) {
        case AbstractTimelineModel::IsGroupRole:
            return true;
        case AbstractTimelineModel::IsInGroupRole:
            return false;
        case AbstractTimelineModel::NumInGroupRole:
            return rowCount(proxyIndex);
        case AbstractTimelineModel::NotificationActorIdentityRole: {
            QVariantList authorList;
            for (int i = 0; i < qMin(rowCount(proxyIndex), 5); ++i) {
                auto identity = index(i, 0, proxyIndex).data(AbstractTimelineModel::NotificationActorIdentityRole);
                authorList.append(identity);
            }
            return authorList;
        }
        }
    } else {
        switch (role) {
        case AbstractTimelineModel::IsGroupRole:
            return false;
        case AbstractTimelineModel::IsInGroupRole:
            return parent.isValid();
        case AbstractTimelineModel::NumInGroupRole:
            return -1;
        }
    }

    return sourceIndex.data(role);
}

QModelIndex NotificationGroupingModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0) {
        return {};
    }

    if (parent.isValid() && row < rowMap.at(parent.row())->count()) {
        return createIndex(row, column, rowMap.at(parent.row()));
    }

    if (row < rowMap.count()) {
        return createIndex(row, column, nullptr);
    }

    return {};
}

QModelIndex NotificationGroupingModel::parent(const QModelIndex &child) const
{
    if (child.internalPointer() == nullptr) {
        return {};
    } else {
        const int parentRow = rowMap.indexOf(static_cast<QVector<int> *>(child.internalPointer()));

        if (parentRow != -1) {
            return index(parentRow, 0, QModelIndex());
        }

        // If we were asked to find the parent for an internalPointer we can't
        // locate, we have corrupted data: This should not happen.
        Q_ASSERT(parentRow != -1);
    }

    return {};
}

QModelIndex NotificationGroupingModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid() || sourceIndex.model() != sourceModel()) {
        return {};
    }

    for (int i = 0; i < rowMap.count(); ++i) {
        const QVector<int> *sourceRows = rowMap.at(i);
        const int childIndex = sourceRows->indexOf(sourceIndex.row());
        const QModelIndex parent = index(i, 0, QModelIndex());

        if (childIndex == 0) {
            // If the sub-list we found the source row in is larger than 1 (i.e. part
            // of a group, map to the logical child item instead of the parent item
            // the source row also stands in for. The parent is therefore unreachable
            // from mapToSource().
            if (isGroup(i)) {
                return index(0, 0, parent);
                // Otherwise map to the top-level item.
            } else {
                return parent;
            }
        } else if (childIndex != -1) {
            return index(childIndex, 0, parent);
        }
    }

    return {};
}

QModelIndex NotificationGroupingModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid() || proxyIndex.model() != this || !sourceModel()) {
        return {};
    }

    const QModelIndex &parent = proxyIndex.parent();

    if (parent.isValid()) {
        if (parent.row() < 0 || parent.row() >= rowMap.count()) {
            return {};
        }

        return sourceModel()->index(rowMap.at(parent.row())->at(proxyIndex.row()), 0);
    } else {
        // Group parents items therefore equate to the first child item; the source
        // row logically appears twice in the proxy.
        // mapFromSource() is not required to handle this well (consider proxies can
        // filter out rows, too) and opts to map to the child item, as the group parent
        // has its Qt::DisplayRole mangled by data(), and it's more useful for trans-
        // lating dataChanged() from the source model.
        // NOTE we changed that to be last
        if (rowMap.isEmpty()) { // FIXME
            // How can this happen? (happens when closing a group)
            return {};
        }
        return sourceModel()->index(rowMap.at(proxyIndex.row())->constLast(), 0);
    }
}

bool NotificationGroupingModel::loading() const
{
    auto notificationModel = (NotificationModel *)sourceModel();
    return notificationModel->loading();
}

void NotificationGroupingModel::setSourceModel(NotificationModel *model)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(model));
    connect(model, &NotificationModel::loadingChanged, this, &NotificationGroupingModel::loadingChanged);
    Q_EMIT sourceModelChanged();
}

NotificationModel *NotificationGroupingModel::getSourceModel()
{
    return qobject_cast<NotificationModel *>(sourceModel());
}
