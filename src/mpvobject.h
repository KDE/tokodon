// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MPVRENDERER_H_
#define MPVRENDERER_H_

#include <QQuickFramebufferObject>

#include "qthelper.hpp"
#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvRenderer;

class MpvObject : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(qreal position READ position NOTIFY positionChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)

    mpv_handle *mpv;
    mpv_render_context *mpv_gl;

    friend class MpvRenderer;

public:
    static void on_update(void *ctx);

    MpvObject(QQuickItem *parent = 0);
    virtual ~MpvObject();
    Renderer *createRenderer() const override;

    qreal position();
    qreal duration();
    bool paused();

public Q_SLOTS:
    void play();
    void pause();
    void stop();
    void setPosition(double value);
    void seek(qreal offset);
    void command(const QVariant &params);
    void setOption(const QString &name, const QVariant &value);
    void setProperty(const QString &name, const QVariant &value);
    QVariant getProperty(const QString &name);

Q_SIGNALS:
    void positionChanged();
    void durationChanged();
    void pausedChanged();
    void onUpdate();

private Q_SLOTS:
    void onMpvEvents();
    void doUpdate();

private:
    bool m_paused = true;
    qreal m_position = 0;
    qreal m_duration = 0;
};

#endif
