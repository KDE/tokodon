// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actiondata.h"
#include "actioncollection.h"

#include <QKeySequence>
#include <QQmlContext>
#include <QQmlProperty>
#include <QVariant>
#include <QtQml/qqmlinfo.h>

#include <KConfigGroup>
#include <KSharedConfig>

static QKeySequence variantToKeySequence(const QVariant &var)
{
    if (var.metaType().id() == QMetaType::Int) {
        return QKeySequence(static_cast<QKeySequence::StandardKey>(var.toInt()));
    }
    return QKeySequence::fromString(var.toString());
}

ActionGroup::ActionGroup(QObject *parent)
    : QActionGroup(parent)
{
}

ActionData::ActionData(QObject *parent)
    : QAction(parent)
{
    connect(this, &ActionData::textChanged, this, &ActionData::syncDown);
    connect(this, &ActionData::checkableChanged, this, &ActionData::syncDown);
    connect(this, &ActionData::toggled, this, &ActionData::syncDown);
}

ActionData::~ActionData()
{
}

QString ActionData::name() const
{
    return m_name;
}

void ActionData::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;

    Q_EMIT nameChanged(name);
}

// QString ActionData::text() const
// {
//     return m_text;
// }
//
// void ActionData::setText(const QString &text)
// {
//     if (m_text == text) {
//         return;
//     }
//
//     m_text = text;
//
//     if (m_action) {
//         m_action->setProperty("text", text);
//     }
//
//     Q_EMIT textChanged(text);
// }

QString ActionData::icon() const
{
    return m_icon;
}

void ActionData::setIcon(const QString &icon)
{
    if (m_icon == icon) {
        return;
    }

    m_icon = icon;

    if (m_action) {
        QQmlProperty property(m_action, QStringLiteral("icon.name"));
        property.write(icon);
    }

    Q_EMIT iconChanged(icon);
}

QVariant ActionData::variantShortcut() const
{
    return m_shortcut;
}

void ActionData::setVariantShortcut(const QVariant &shortcut)
{
    if (m_shortcut == shortcut) {
        return;
    }

    m_shortcut = shortcut;

    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Shortcuts"));
    cg = KConfigGroup(&cg, m_collection->name());
    qWarning() << "AAA" << shortcut << m_defaultShortcut << (shortcut == m_defaultShortcut);
    if (shortcut != m_defaultShortcut) {
        cg.writeEntry(m_name, variantToKeySequence(shortcut).toString());
    } else {
        cg.deleteEntry(m_name);
    }

    if (m_action) {
        m_action->setProperty("shortcut", shortcut);
    }

    Q_EMIT shortcutChanged(shortcut);
}

QVariant ActionData::defaultShortcut() const
{
    return m_defaultShortcut;
}

void ActionData::setDefaultShortcut(const QVariant &shortcut)
{
    if (m_defaultShortcut == shortcut) {
        return;
    }

    m_defaultShortcut = shortcut;

    Q_EMIT defaultShortcutChanged(shortcut);
}

void ActionData::setActionGroupNotify(QActionGroup *group)
{
    setActionGroup(group);
    Q_EMIT actionGroupChanged(group);
}

QObject *ActionData::action() const
{
    return m_action;
}

void ActionData::setAction(QObject *action)
{
    if (m_action == action) {
        return;
    }

    if (m_action) {
        disconnect(m_action, nullptr, this, nullptr);
        disconnect(this, nullptr, m_action, nullptr);
    }
    m_action = action;

    connect(this, &ActionData::toggled, m_action, [this](bool checked) {
        m_action->setProperty("checked", checked);
    });
    connect(m_action, SIGNAL(toggled()), this, SLOT(syncUp()));

    syncDown();

    Q_EMIT actionChanged(action);
}

void ActionData::syncDown()
{
    if (m_action) {
        m_action->setProperty("text", text());
        QQmlProperty property(m_action, QStringLiteral("icon.name"));
        property.write(m_icon);
        m_action->setProperty("shortcut", m_shortcut);
        m_action->setProperty("checkable", isCheckable());
        m_action->setProperty("checked", isChecked());
    }
}

void ActionData::syncUp()
{
    if (m_action) {
        setChecked(m_action->property("checked").toBool());
    }
}

void ActionData::classBegin()
{
    m_collection = qobject_cast<ActionCollection *>(parent());
    if (!m_collection) {
        QQmlError error;
        error.setDescription(QStringLiteral("ActionData must be a child of ActionCollection."));
        error.setMessageType(QtFatalMsg);
        QQmlContext *context = qmlContext(this);
        if (context) {
            error.setUrl(context->baseUrl());
        }
        qmlWarning(nullptr, error);
        qFatal();
    }
}

void ActionData::componentComplete()
{
    if (!m_collection) {
        return;
    }
    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Shortcuts"));
    cg = KConfigGroup(&cg, m_collection->name());
    QString shortcut = variantToKeySequence(m_defaultShortcut).toString();
    shortcut = cg.readEntry(m_name, shortcut);

    if (shortcut != m_shortcut.toString()) {
        m_shortcut = shortcut;
        Q_EMIT shortcutChanged(m_shortcut);
    }
    syncDown();
}

#include "moc_actiondata.cpp"
