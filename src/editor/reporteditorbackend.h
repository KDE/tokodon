// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>
#include <memory>

#include "timeline/post.h"

class ReportEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString accountId MEMBER m_accountId NOTIFY accountIdChanged)
    Q_PROPERTY(QString postId MEMBER m_postId NOTIFY postIdChanged)
    Q_PROPERTY(QString comment MEMBER m_comment NOTIFY commentChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit ReportEditorBackend(QObject *parent = nullptr);

    bool loading() const;

public Q_SLOTS:
    void submit();

Q_SIGNALS:
    void accountIdChanged();
    void postIdChanged();
    void commentChanged();
    void reported();
    void loadingChanged();

private:
    QString m_accountId;
    QString m_postId;
    QString m_comment;
    bool m_loading = false;
};
