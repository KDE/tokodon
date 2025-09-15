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

///////////////////////////////////

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

    connect(collection, &QObject::destroyed, this, [this, collection]() {
        m_collections.remove(collection->name());
        Q_EMIT collectionRemoved(collection);
    });

    Q_EMIT collectionInserted(collection);
}

ActionCollection *ActionCollections::collection(const QString &name)
{
    return m_collections.value(name);
}

QList<ActionCollection *> ActionCollections::collections()
{
    return m_collections.values();
}

#include "moc_actioncollection.cpp"
