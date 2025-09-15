// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef ACTIONMODEL_H
#define ACTIONMODEL_H

#include <QAbstractListModel>
#include <qqmlregistration.h>

#include "actiondata.h"

class ActionModelPrivate;

class ActionModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionModel)
    // TODO: this should be able to have multiple collections? or just a KConcatenateProxyModel?
    Q_PROPERTY(QString collectionName READ collectionName WRITE setCollectionName NOTIFY collectionNameChanged FINAL)
    Q_PROPERTY(ShownActions shownActions READ shownActions WRITE setShownActions NOTIFY shownActionsChanged FINAL)

public:
    enum ShownActions {
        AllActions = 0,
        ActiveActions
    };
    Q_ENUM(ShownActions);

    enum Role {
        ActionDescriptionRole = Qt::UserRole + 1,
        ActionInstanceRole
    };

    explicit ActionModel(QObject *parent = nullptr);
    ~ActionModel() override;

    QString collectionName() const;
    void setCollectionName(const QString &name);

    ShownActions shownActions() const;
    void setShownActions(ShownActions shown);

    ActionCollection *collection() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void collectionNameChanged(const QString &name);
    void shownActionsChanged(ShownActions shown);

private:
    QPointer<ActionCollection> m_collection;
    ActionModelPrivate *d = nullptr;
    friend class ActionModelPrivate;
};

#endif
