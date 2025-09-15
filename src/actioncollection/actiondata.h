// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

#include <QAction>
#include <QActionGroup>
#include <QQmlEngine>
#include <QVariant>
#include <qnamespace.h>
#include <qqmlregistration.h>

#include <QKeySequence>

class ActionCollection;
class ActionData;
class IconGroupPrivate;

class ActionGroup : public QActionGroup
{
    Q_OBJECT
    QML_ELEMENT
public:
    ActionGroup(QObject *parent = nullptr);
};

class IconGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)

public:
    IconGroup(ActionData *parent = nullptr);
    ~IconGroup();

    QString name() const;
    void setName(const QString &name);

    QString source() const;
    void setSource(const QString &source);

    qreal width() const;
    void setWidth(qreal width);

    qreal height() const;
    void setHeight(qreal height);

    QColor color() const;
    void setColor(const QColor &color);

    bool cache() const;
    void setCache(bool cache);

Q_SIGNALS:
    void nameChanged();
    void sourceChanged();
    void widthChanged();
    void heightChanged();
    void colorChanged();
    void cacheChanged();

private:
    std::unique_ptr<IconGroupPrivate> d;
};

// Accessible from both C++ and QML
class ActionData : public QAction, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    // Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)
    Q_PROPERTY(IconGroup *icon READ icon CONSTANT FINAL)
    Q_PROPERTY(QVariant defaultShortcut READ defaultShortcut WRITE setDefaultShortcut NOTIFY defaultShortcutChanged FINAL)
    Q_PROPERTY(QVariant shortcut READ variantShortcut WRITE setVariantShortcut NOTIFY shortcutChanged FINAL)
    // Q_PROPERTY(QList<QKeySequence> alternateShortcuts READ alternateShortcuts WRITE setShortcuts NOTIFY alternateShortcutsChanged)
    //  TODO: checkable? other Action properties to proxy? proxy trigger and triggered? at this poing might well be a QAction?
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
    IconGroup *icon() const;

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
    void alternateShortcutsChanged();
    void defaultShortcutChanged(const QVariant &defaultShortcut);
    void actionGroupChanged(QActionGroup *group);
    void actionChanged(QObject *action);

private Q_SLOTS:
    void syncUp();

private:
    // TODO: dpointer
    void syncDown();
    IconGroup *m_icon;
    QString m_name;
    QString m_text;
    QVariant m_shortcut = QString();
    QVariant m_defaultShortcut;
    QPointer<QObject> m_action;
    ActionCollection *m_collection;
};
