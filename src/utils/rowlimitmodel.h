#pragma once

#include <QAbstractItemModel>
#include <QJSValue>
#include <QList>
#include <QQmlParserStatus>
#include <QSortFilterProxyModel>
#include <qqmlregistration.h>

#include <array>

/**
 * @class SortFilterModel
 * @short Filter and sort an existing QAbstractItemModel
 *
 * @since 5.67
 */
class RowLimitModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit RowLimitModel(QObject *parent = nullptr);
    ~RowLimitModel() override;

    void setSourceModel(QAbstractItemModel *sourceModel) override;
    ;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
};
