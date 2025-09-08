// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

#include <QAction>
#include <QActionGroup>
#include <QQmlEngine>
#include <QVariant>
#include <qqmlregistration.h>

#include <QKeySequence>

class ActionCollection;

class ActionGroup : public QActionGroup
{
    Q_OBJECT
    QML_ELEMENT
public:
    ActionGroup(QObject *parent = nullptr);
};

// Accessible from both C++ and QML
class ActionData : public QAction, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    // Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged FINAL)
    Q_PROPERTY(QVariant defaultShortcut READ defaultShortcut WRITE setDefaultShortcut NOTIFY defaultShortcutChanged FINAL)
    Q_PROPERTY(QVariant shortcut READ variantShortcut WRITE setVariantShortcut NOTIFY shortcutChanged FINAL)
    // TODO: checkable? other Action properties to proxy? proxy trigger and triggered? at this poing might well be a QAction?
    Q_PROPERTY(QObject *action READ action() WRITE setAction NOTIFY actionChanged)
    Q_PROPERTY(QActionGroup *actionGroup READ actionGroup WRITE setActionGroupNotify NOTIFY actionGroupChanged)
public:
    explicit ActionData(QObject *parent = nullptr);
    ~ActionData() override;

    QString name() const;
    void setName(const QString &name);

    // QString text() const;
    // void setText(const QString &text);

    // TODO: replace with the usual grouped property
    QString icon() const;
    void setIcon(const QString &name);

    QVariant variantShortcut() const;
    void setVariantShortcut(const QVariant &shortcut);

    QVariant defaultShortcut() const;
    void setDefaultShortcut(const QVariant &shortcut);

    void setActionGroupNotify(QActionGroup *group);

    QObject *action() const;
    void setAction(QObject *action);

    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void textChanged(const QString &text);
    void iconChanged(const QString &icon);
    void shortcutChanged(const QVariant &shortcut);
    void defaultShortcutChanged(const QVariant &defaultShortcut);
    void actionGroupChanged(QActionGroup *group);
    void actionChanged(QObject *action);

private:
    // TODO: dpointer
    void syncDown();
    void syncUp();
    QString m_name;
    QString m_text;
    QString m_icon;
    QVariant m_shortcut;
    QVariant m_defaultShortcut;
    QPointer<QObject> m_action;
    ActionCollection *m_collection;
};
