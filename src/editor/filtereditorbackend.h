// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/post.h"

class FilterEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString filterId READ filterId WRITE setFilterId NOTIFY filterIdChanged)
    Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)

public:
    explicit FilterEditorBackend(QObject *parent = nullptr);

    [[nodiscard]] QString filterId() const;
    void setFilterId(const QString &filterId);

    [[nodiscard]] bool loading() const;

public Q_SLOTS:
    void submit();
    void deleteFilter();

Q_SIGNALS:
    void filterIdChanged();
    void titleChanged();
    void done();
    void loadingChanged();

private:
    QString m_filterId;
    QString m_title;
    bool m_loading = false;
};
