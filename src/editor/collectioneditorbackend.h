// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "datatypes/post.h"

class CollectionEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString collectionId READ collectionId WRITE setCollectionId NOTIFY collectionIdChanged)
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QString description MEMBER m_description NOTIFY descriptionChanged)
    Q_PROPERTY(bool sensitive MEMBER m_sensitive NOTIFY sensitiveChanged)
    Q_PROPERTY(bool discoverable MEMBER m_discoverable NOTIFY discoverableChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit CollectionEditorBackend(QObject *parent = nullptr);

    [[nodiscard]] QString collectionId() const;
    void setCollectionId(const QString &collectionId);

    [[nodiscard]] bool loading() const;

public Q_SLOTS:
    void submit();
    void deleteCollection();

Q_SIGNALS:
    void collectionIdChanged();
    void nameChanged();
    void done();
    void loadingChanged();
    void descriptionChanged();
    void sensitiveChanged();
    void discoverableChanged();

private:
    QString m_collectionId;
    QString m_name;
    bool m_loading = false;
    QString m_description;
    bool m_sensitive = false;
    bool m_discoverable = true;
};
