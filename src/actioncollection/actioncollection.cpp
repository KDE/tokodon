// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actioncollection.h"
#include "actiondata.h"

#include <QKeySequence>
#include <QQmlContext>
#include <QQmlProperty>
#include <QtQml/qqmlinfo.h>

Q_GLOBAL_STATIC(ActionCollections, s_actionCollectionStorage)

ActionCollectionAttached::ActionCollectionAttached(QObject *parent)
    : QObject(parent)
{
}

ActionCollectionAttached::~ActionCollectionAttached()
{
}

QString ActionCollectionAttached::collection() const
{
    return m_collection;
}

void ActionCollectionAttached::setCollection(const QString &collection)
{
    if (m_collection == collection)
        return;

    m_collection = collection;

    ActionCollection *coll = ActionCollections::self()->collection(collection);
    const QString name = parent()->objectName();

    if (coll && !name.isEmpty()) {
        ActionData *ad = coll->action(name);
        if (ad) {
            ad->setAction(parent());
        }
    }

    Q_EMIT collectionChanged();
}

//////////////////////////////////

QString decodeStandardShortcut(const QString &shortcut)
{
    // Is the default shortcut a named one, such as "Copy", "Back" etc ?
    QMetaEnum me = QMetaEnum::fromType<QKeySequence::StandardKey>();
    for (int i = 0; i < me.keyCount(); ++i) {
        if (QString::fromUtf8(me.key(i)).toLower() == shortcut) {
            return QKeySequence(QKeySequence::StandardKey(me.value(i))).toString();
        }
    }

    return shortcut;
}

ActionCollection::ActionCollection(QObject *parent)
    : QObject(parent)
{
}

ActionCollection::~ActionCollection()
{
}

QString ActionCollection::name() const
{
    return m_name;
}

void ActionCollection::setName(const QString &name)
{
    if (name == m_name) {
        return;
    }

    m_name = name;

    ActionCollections::self()->insertCollection(this);

    Q_EMIT nameChanged(name);
}

ActionData *ActionCollection::action(const QString &name)
{
    return m_actionMap.value(name);
}

QList<ActionData *> ActionCollection::actions() const
{
    return m_actions;
}

QList<ActionData *> ActionCollection::activeActions() const
{
    // TODO
    return m_actions;
}

ActionCollectionAttached *ActionCollection::qmlAttachedProperties(QObject *object)
{
    return new ActionCollectionAttached(object);
}

void ActionCollection::actions_append(QQmlListProperty<ActionData> *prop, ActionData *action)
{
    // This can only be called from QML
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return;
    }

    const QString name = action->name();
    if (coll->m_actionMap.contains(name) && !name.isEmpty()) { // FIXME: name should be checked later
        QQmlError error;
        error.setDescription(QStringLiteral("ActionData name ") % name % QStringLiteral(" is not unique"));
        error.setMessageType(QtFatalMsg);
        QQmlContext *context = qmlContext(coll);
        if (context) {
            error.setUrl(context->baseUrl());
        }
        qmlWarning(nullptr, error);
        qFatal();
    }
    coll->m_actions.append(action);
    coll->m_actionMap[name] = action;

    connect(action, &QObject::destroyed, coll, [coll, action, name]() {
        coll->m_actionMap.remove(name);
        coll->m_actions.removeAll(action);
    });

    if (action->action()) {
        Q_ASSERT(!coll->m_activeActions.contains(action));
        const int pos = coll->m_activeActions.count();
        Q_EMIT coll->aboutToAddActionInstance(pos, action);
        coll->m_activeActions.append(action);
        Q_EMIT coll->actionInstanceAdded(pos, action);
    }

    connect(action, &ActionData::actionChanged, coll, [coll, action](QObject *actionInstance) {
        if (actionInstance) {
            Q_ASSERT(!coll->m_activeActions.contains(action));
            const int pos = coll->m_activeActions.count();
            Q_EMIT coll->aboutToAddActionInstance(pos, action);
            coll->m_activeActions.append(action);
            Q_EMIT coll->actionInstanceAdded(pos, action);
        } else {
            const int pos = coll->m_activeActions.indexOf(action);
            Q_ASSERT(pos >= 0);
            Q_EMIT coll->aboutToRemoveActionInstance(pos, action);
            coll->m_activeActions.removeAll(action);
            Q_EMIT coll->actionInstanceRemoved(pos, action);
        }
    });

    action->setParent(coll);
}

qsizetype ActionCollection::actions_count(QQmlListProperty<ActionData> *prop)
{
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return 0;
    }

    return coll->m_actions.count();
}

ActionData *ActionCollection::actions_at(QQmlListProperty<ActionData> *prop, qsizetype index)
{
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return nullptr;
    }

    if (index < 0 || index >= coll->m_actions.count()) {
        return nullptr;
    }
    return coll->m_actions.value(index);
}

void ActionCollection::actions_clear(QQmlListProperty<ActionData> *prop)
{
    ActionCollection *coll = static_cast<ActionCollection *>(prop->object);
    if (!coll) {
        return;
    }

    coll->m_actionMap.clear();
    coll->m_actions.clear();
}

QQmlListProperty<ActionData> ActionCollection::actionsProperty()
{
    return QQmlListProperty<ActionData>(this, nullptr, actions_append, actions_count, actions_at, actions_clear);
}

/////////////////////////////////

ActionsModel::ActionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ActionsModel::~ActionsModel()
{
}

QString ActionsModel::collectionName() const
{
    if (m_collection) {
        return m_collection->name();
    }

    return {};
}

void ActionsModel::setCollectionName(const QString &name)
{
    if (m_collection && name == m_collection->name()) {
        return;
    }

    if (m_collection) {
        disconnect(m_collection, nullptr, this, nullptr);
    }

    m_collection = ActionCollections::self()->collection(name);

    connect(m_collection, &ActionCollection::aboutToAddActionInstance, this, [this](int position, QObject *action) {
        if (m_shownActions != ActiveActions) {
            return;
        }
        beginInsertRows(QModelIndex(), position, position);
    });
    connect(m_collection, &ActionCollection::actionInstanceAdded, this, [this](int position, QObject *action) {
        if (m_shownActions != ActiveActions) {
            return;
        }
        endInsertRows();
    });
    connect(m_collection, &ActionCollection::aboutToRemoveActionInstance, this, [this](int position, QObject *action) {
        if (m_shownActions != ActiveActions) {
            return;
        }
        beginRemoveRows(QModelIndex(), position, position);
    });
    connect(m_collection, &ActionCollection::actionInstanceRemoved, this, [this](int position, QObject *action) {
        Q_UNUSED(position);
        Q_UNUSED(action);
        if (m_shownActions != ActiveActions) {
            return;
        }
        endRemoveRows();
    });

    Q_EMIT collectionNameChanged(m_collection ? name : QString());
}

ActionsModel::ShownActions ActionsModel::shownActions() const
{
    return m_shownActions;
}

void ActionsModel::setShownActions(ShownActions shown)
{
    if (shown == m_shownActions) {
        return;
    }

    beginResetModel();
    m_shownActions = shown;
    endResetModel();

    Q_EMIT shownActionsChanged(shown);
}

ActionCollection *ActionsModel::collection() const
{
    return m_collection;
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    if (!m_collection || parent.isValid()) {
        return 0;
    }

    if (m_shownActions == ActiveActions) {
        return m_collection->activeActions().count();
    }
    return m_collection->actions().count();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    if (!m_collection) {
        return {};
    }

    ActionData *action;
    if (m_shownActions == ActiveActions) {
        action = m_collection->activeActions()[index.row()];
    } else {
        action = m_collection->actions()[index.row()];
    }

    switch (role) {
    case Qt::DisplayRole:
        return action->text();
    case ActionDescriptionRole:
        return QVariant::fromValue(action);
    case ActionInstanceRole:
        return QVariant::fromValue(action->action());
    }

    return {};
}

QHash<int, QByteArray> ActionsModel::roleNames() const
{
    return {{Qt::DisplayRole, "display"}, {ActionDescriptionRole, "actionDescription"}, {ActionInstanceRole, "actionInstance"}};
}

/////////////////////////////////

ActionCollections::ActionCollections(QObject *parent)
    : QObject(parent)
{
}

ActionCollections::~ActionCollections()
{
}

ActionCollections *ActionCollections::self()
{
    return s_actionCollectionStorage;
}

void ActionCollections::insertCollection(ActionCollection *collection)
{
    m_collections.insert(collection->name(), collection);
}

ActionCollection *ActionCollections::collection(const QString &name)
{
    return m_collections.value(name);
}

#include "moc_actioncollection.cpp"
