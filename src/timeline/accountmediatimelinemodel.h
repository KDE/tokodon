// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timeline/abstractlistmodel.h"

class AbstractAccount;

/**
 * @brief TimelineModel to show the media of an account.
 */
class AccountMediaTimelineModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @brief The account id of the account we want to display.
     */
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)

    /**
     * @brief Search for posts with a specific hashtag, leave blank to not search for any.
     */
    Q_PROPERTY(QString tagged MEMBER m_tagged NOTIFY filtersChanged)

public:
    explicit AccountMediaTimelineModel(QObject *parent = nullptr);

    QString displayName() const override;

    [[nodiscard]] QString accountId() const;
    void setAccountId(const QString &accountId);

    void fillTimeline(const QString &fromId = {});

    enum CustomRoles {
        PostIdRole = Qt::UserRole,
        SourceRole,
        TempSourceRole,
        FocusXRole,
        FocusYRole,
        SensitiveRole,
        AttachmentRole,
    };

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void accountIdChanged();
    void filtersChanged();
    void tabChanged();

protected:
    void reset();
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    std::shared_ptr<Identity> m_identity;
    QString m_accountId;

    QString m_tagged;

    struct MediaAttachment {
        QString postId;
        Attachment *attachment = nullptr;
        bool sensitive;
    };

    QList<MediaAttachment> m_timeline;
};
