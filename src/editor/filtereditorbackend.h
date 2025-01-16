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
    Q_PROPERTY(bool homeAndListsContext MEMBER m_homeAndListsContext NOTIFY homeAndListsContextChanged)
    Q_PROPERTY(bool notificationsContext MEMBER m_notificationsContext NOTIFY notificationsContextChanged)
    Q_PROPERTY(bool publicTimelinesContext MEMBER m_publicTimelinesContext NOTIFY publicTimelinesContextChanged)
    Q_PROPERTY(bool conversationsContext MEMBER m_conversationsContext NOTIFY conversationsContextChanged)
    Q_PROPERTY(bool profilesContext MEMBER m_profilesContext NOTIFY profilesContextChanged)

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
    void homeAndListsContextChanged();
    void notificationsContextChanged();
    void publicTimelinesContextChanged();
    void conversationsContextChanged();
    void profilesContextChanged();

private:
    QString m_filterId;
    QString m_title;
    bool m_loading = false;
    bool m_homeAndListsContext = false;
    bool m_notificationsContext = false;
    bool m_publicTimelinesContext = false;
    bool m_conversationsContext = false;
    bool m_profilesContext = false;
};
