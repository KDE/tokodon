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

class IconGroupPrivate
{
public:
    IconGroupPrivate(IconGroup *group)
        : q(group)
    {
    }

    IconGroup *q;
    ActionData *m_ad = nullptr;
    QString m_name;
    QString m_source;
    qreal m_width = -1;
    qreal m_height = -1;
    QColor m_color = Qt::transparent;
    bool m_cache = true;
};

IconGroup::IconGroup(ActionData *parent)
    : QObject(parent)
    , d(std::make_unique<IconGroupPrivate>(this))
{
    d->m_ad = parent;
}

IconGroup::~IconGroup()
{
}

QString IconGroup::name() const
{
    return d->m_name;
}

void IconGroup::setName(const QString &name)
{
    if (d->m_name == name) {
        return;
    }

    d->m_name = name;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.name"));
        property.write(name);
    }

    Q_EMIT nameChanged();
}

QString IconGroup::source() const
{
    return d->m_source;
}

void IconGroup::setSource(const QString &source)
{
    if (d->m_source != source) {
        return;
    }

    d->m_source = source;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.source"));
        property.write(source);
    }

    Q_EMIT sourceChanged();
}

qreal IconGroup::width() const
{
    return d->m_width;
}

void IconGroup::setWidth(qreal width)
{
    if (qFuzzyCompare(d->m_width, width)) {
        return;
    }

    d->m_width = width;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.width"));
        property.write(width);
    }

    Q_EMIT widthChanged();
}

qreal IconGroup::height() const
{
    return d->m_height;
}

void IconGroup::setHeight(qreal height)
{
    if (qFuzzyCompare(d->m_height, height)) {
        return;
    }

    d->m_height = height;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.height"));
        property.write(height);
    }

    Q_EMIT heightChanged();
}

QColor IconGroup::color() const
{
    return d->m_color;
}

void IconGroup::setColor(const QColor &color)
{
    if (d->m_color == color) {
        return;
    }

    d->m_color = color;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.color"));
        property.write(color);
    }

    Q_EMIT colorChanged();
}

bool IconGroup::cache() const
{
    return d->m_cache;
}

void IconGroup::setCache(bool cache)
{
    if (d->m_cache == cache) {
        return;
    }

    d->m_cache = cache;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.cache"));
        property.write(cache);
    }

    Q_EMIT cacheChanged();
}

ActionData::ActionData(QObject *parent)
    : QAction(parent)
{
    m_icon = new IconGroup(this);
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

IconGroup *ActionData::icon() const
{
    return m_icon;
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
        // QQmlProperty property(m_action, QStringLiteral("icon.name"));
        // property.write(m_icon);
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
