// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <QObject>
#include <QQmlEngine>
#include <fontconfig/fontconfig.h>
#include <qqmlregistration.h>

#include "actiondata.h"

class ActionCollectionAttached : public QObject
{
    Q_OBJECT
    // QML_NAMED_ELEMENT(ActionCollection)
    // QML_ATTACHED(ActionCollectionAttached)
    // QML_UNCREATABLE("Cannot create objects of type ActionCollectionAttached, use it as an attached property")

    Q_PROPERTY(QString collection READ collection WRITE setCollection NOTIFY collectionChanged FINAL)

public:
    explicit ActionCollectionAttached(QObject *parent = nullptr);
    ~ActionCollectionAttached() override;

    QString collection() const;
    void setCollection(const QString &collection);

Q_SIGNALS:
    void collectionChanged();
    void shortcutChanged();

private:
    QString m_collection;
};

// Accessible from both C++ and QML
class ActionCollection : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(ActionCollectionAttached)

    // FIXME: use just objectName?
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    // Needs a signal? (this only if we want to provide add and remove from c++)
    Q_PROPERTY(QQmlListProperty<ActionData> actions READ actionsProperty CONSTANT FINAL)

    Q_CLASSINFO("DefaultProperty", "actions")

public:
    explicit ActionCollection(QObject *parent = nullptr);
    ~ActionCollection() override;

    QString name() const;
    void setName(const QString &name);

    Q_INVOKABLE ActionData *action(const QString &name);
    // All known actions in this collection
    QList<ActionData *> actions() const;
    // All actions in this collections that have an active and working Kirigami.Action instance in this moment
    QList<ActionData *> activeActions() const;

    QQmlListProperty<ActionData> actionsProperty();

    // QML attached property
    static ActionCollectionAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void nameChanged(const QString &name);

    void aboutToAddActionInstance(int position, ActionData *action);
    void actionInstanceAdded(int position, ActionData *action);
    void aboutToRemoveActionInstance(int position, ActionData *action);
    void actionInstanceRemoved(int position, ActionData *action);

private:
    // TODO: wonder if all of this should be in a qml-only subclass
    static void actions_append(QQmlListProperty<ActionData> *prop, ActionData *object);
    static qsizetype actions_count(QQmlListProperty<ActionData> *prop);
    static ActionData *actions_at(QQmlListProperty<ActionData> *prop, qsizetype index);
    static void actions_clear(QQmlListProperty<ActionData> *prop);

    // TODO: dpointer
    QString m_name;
    QHash<QString, ActionData *> m_actionMap;
    QList<ActionData *> m_actions;
    QList<ActionData *> m_activeActions;
};

QML_DECLARE_TYPEINFO(ActionCollectionAttached, QML_HAS_ATTACHED_PROPERTIES)

// C++ only api TODO: hide all of this behind a single static of ActionCollection
class ActionCollections : public QObject
{
    Q_OBJECT

public:
    explicit ActionCollections(QObject *parent = nullptr);
    ~ActionCollections() override;

    static ActionCollections *self();

    void insertCollection(ActionCollection *collection);
    ActionCollection *collection(const QString &name);
    QList<ActionCollection *> collections();

Q_SIGNALS:
    void collectionInserted(ActionCollection *collection);
    void collectionRemoved(ActionCollection *collection);

private:
    // QMap as we want a deterministic order
    QMap<QString, ActionCollection *> m_collections;
};

#endif
