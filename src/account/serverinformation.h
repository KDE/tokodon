// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "abstractaccount.h"

#include <QObject>
#include <qqmlintegration.h>

/**
 * @brief View an instance's server information (e.g. privacy policy.)
 */
class ServerInformation : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QString updatedAt READ updatedAt NOTIFY privacyPolicyChanged)
    Q_PROPERTY(QString content READ content NOTIFY privacyPolicyChanged)
    Q_PROPERTY(QString kind READ kind WRITE setKind NOTIFY kindChanged REQUIRED)

public:
    explicit ServerInformation(QObject *parent = nullptr);

    AbstractAccount *account() const;
    void setAccount(AbstractAccount *account);

    QString updatedAt() const;
    QString content() const;
    QString kind() const;
    void setKind(const QString &kind);

Q_SIGNALS:
    void accountChanged();
    void privacyPolicyChanged();
    void kindChanged();

private:
    void fetch();

    AbstractAccount *m_account = nullptr;
    QDateTime m_updatedAt;
    QString m_content;
    QString m_kind;
};
