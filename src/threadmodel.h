// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timelinemodel.h"

class ThreadModel : public TimelineModel
{
    Q_OBJECT
public:
    ThreadModel(AccountManager *manager, const QString &id, QObject *parent = nullptr);

    QString displayName() const override;
    void fillTimeline(QString fromId = QString()) override;
    bool canFetchMore(const QModelIndex &parent) const override;
};

