// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timeline/timelinemodel.h"

/**
 * @brief Model for displaying and organizing post threads
 * @see TimelineModel
 */
class ThreadModel : public TimelineModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString postId READ postId WRITE setPostId NOTIFY postIdChanged)
    Q_PROPERTY(QString postUrl READ postUrl NOTIFY postUrlChanged)
    Q_PROPERTY(bool hasHiddenReplies READ hasHiddenReplies NOTIFY hasHiddenRepliesChanged)

public:
    explicit ThreadModel(QObject *parent = nullptr);

    /**
     * @return The post id of the "root" post of the thread
     * @see setPostId()
     */
    [[nodiscard]] QString postId() const;

    ///
    /// \see postId
    /**
     * @brief Set the post id of the "root" post of the thread.
     * @see postId()
     */
    void setPostId(const QString &postId);

    /**
     * @return The original post url of the "root" post of the thread.
     */
    [[nodiscard]] QString postUrl() const;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QString displayName() const override;
    void fillTimeline(const QString &fromId = QString()) override;
    [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;

    /**
     * @return Returns the index of the root post in the model. Can be used to find where exactly to position the view.
     */
    Q_INVOKABLE [[nodiscard]] int getRootIndex() const;

    void reset() override;

    /**
     * @brief Resets and refreshes the timeline for new posts.
     */
    Q_INVOKABLE void refresh();

    /**
     * @return Whether the post may have replies hidden from the server, but available on the original
     */
    [[nodiscard]] bool hasHiddenReplies() const;

Q_SIGNALS:
    void postIdChanged();
    void postUrlChanged();
    void hasHiddenRepliesChanged();

private:
    QString m_postId, m_postUrl;
    bool m_hasHiddenReplies = false;
    qsizetype m_rootPostIndex = 0;

    friend class TimelineTest;
};
