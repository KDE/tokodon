// SPDX-FileCopyrightText: 2022 Jeremy Winter <jeremy.winter@tutanota.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QJsonObject>

class Identity;

class Relationship : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool following READ following WRITE setFollowing NOTIFY followingChanged)
    Q_PROPERTY(bool requested READ requested WRITE setRequested NOTIFY requestedChanged)
    Q_PROPERTY(bool endorsed READ endorsed WRITE setEndorsed NOTIFY endorsedChanged)
    Q_PROPERTY(bool followedBy READ followedBy WRITE setFollowedBy NOTIFY followedByChanged)
    Q_PROPERTY(bool muting READ muting WRITE setMuting NOTIFY mutingChanged)
    Q_PROPERTY(bool mutingNotifications READ mutingNotifications WRITE setMutingNotifications NOTIFY mutingNotificationsChanged)
    Q_PROPERTY(bool showingReblogs READ showingReblogs WRITE setShowingReblogs NOTIFY showingReblogsChanged)
    Q_PROPERTY(bool notifying READ notifying WRITE setNotifying NOTIFY notifyingChanged)
    Q_PROPERTY(bool blocking READ blocking WRITE setBlocking NOTIFY blockingChanged)
    Q_PROPERTY(bool domainBlocking READ domainBlocking WRITE setDomainBlocking NOTIFY domainBlockingChanged)
    Q_PROPERTY(bool blockedBy READ blockedBy WRITE setBlockedBy NOTIFY blockedByChanged)
    Q_PROPERTY(QString note READ note WRITE setNote NOTIFY noteChanged)

public:
    explicit Relationship(Identity *parent, const QJsonObject &jsonObj);

    void updateFromJson(const QJsonObject &jsonObj);

    Identity *m_parent;

    [[nodiscard]] bool following() const;
    void setFollowing(bool following);
    [[nodiscard]] bool requested() const;
    void setRequested(bool requested);
    [[nodiscard]] bool endorsed() const;
    void setEndorsed(bool endorsed);
    [[nodiscard]] bool followedBy() const;
    void setFollowedBy(bool followedBy);
    [[nodiscard]] bool muting() const;
    void setMuting(bool muting);
    [[nodiscard]] bool mutingNotifications() const;
    void setMutingNotifications(bool mutingNotifications);
    [[nodiscard]] bool showingReblogs() const;
    void setShowingReblogs(bool showingReblogs);
    [[nodiscard]] bool notifying() const;
    void setNotifying(bool notifying);
    [[nodiscard]] bool blocking() const;
    void setBlocking(bool blocking);
    [[nodiscard]] bool domainBlocking() const;
    void setDomainBlocking(bool domainBlocking);
    [[nodiscard]] bool blockedBy() const;
    void setBlockedBy(bool blockedBy);
    [[nodiscard]] QString note() const;
    void setNote(const QString &note);

Q_SIGNALS:
    void followingChanged();
    void requestedChanged();
    void endorsedChanged();
    void followedByChanged();
    void mutingChanged();
    void mutingNotificationsChanged();
    void showingReblogsChanged();
    void notifyingChanged();
    void blockingChanged();
    void domainBlockingChanged();
    void blockedByChanged();
    void noteChanged();

private:
    bool m_following = false;
    bool m_requested = false;
    bool m_endorsed = false;
    bool m_followedBy = false;
    bool m_muting = false;
    bool m_mutingNotifications = false;
    bool m_showingReblogs = false;
    bool m_notifying = false;
    bool m_blocking = false;
    bool m_domainBlocking = false;
    bool m_blockedBy = false;
    QString m_note;
};
