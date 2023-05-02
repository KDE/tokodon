// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mpvplayer.h"

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QStandardPaths>
#include <stdexcept>

namespace
{
void on_mpv_events(void *ctx)
{
    QMetaObject::invokeMethod((MpvPlayer *)ctx, "onMpvEvents", Qt::QueuedConnection);
}

void on_mpv_redraw(void *ctx)
{
    MpvPlayer::on_update(ctx);
}

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx)

    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;

    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

}

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
    MpvPlayer *obj = nullptr;

public:
    MpvRenderer(MpvPlayer *new_obj)
        : obj{new_obj}
    {
    }

    virtual ~MpvRenderer()
    {
    }

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        // init mpv_gl:
        if (!obj->mpv_gl) {
            mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
            mpv_render_param params[]{{MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
                                      {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
                                      {MPV_RENDER_PARAM_INVALID, nullptr}};

            if (mpv_render_context_create(&obj->mpv_gl, obj->mpv, params) < 0)
                throw std::runtime_error("failed to initialize mpv GL context");
            mpv_render_context_set_update_callback(obj->mpv_gl, on_mpv_redraw, obj);
        }

        return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
    }

    void render() override
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        obj->window()->resetOpenGLState();
#endif

        QOpenGLFramebufferObject *fbo = framebufferObject();
        mpv_opengl_fbo mpfbo;
        mpfbo.fbo = static_cast<int>(fbo->handle());
        mpfbo.w = fbo->width();
        mpfbo.h = fbo->height();
        mpfbo.internal_format = 0;

        int flip_y = 0;

        mpv_render_param params[] = {// Specify the default framebuffer (0) as target. This will
                                     // render onto the entire screen. If you want to show the video
                                     // in a smaller rectangle or apply fancy transformations, you'll
                                     // need to render into a separate FBO and draw it manually.
                                     {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
                                     // Flip rendering (needed due to flipped GL coordinate system).
                                     {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
                                     {MPV_RENDER_PARAM_INVALID, nullptr}};
        // See render_gl.h on what OpenGL environment mpv expects, and
        // other API details.
        mpv_render_context_render(obj->mpv_gl, params);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        obj->window()->resetOpenGLState();
#endif
    }
};

MpvPlayer::MpvPlayer(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , mpv{mpv_create()}
    , mpv_gl(nullptr)
{
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

    // enable console output
    mpv_set_option_string(mpv, "terminal", "yes");

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "pause", MPV_FORMAT_FLAG);

    mpv_set_wakeup_callback(mpv, on_mpv_events, this);

    connect(this, &MpvPlayer::onUpdate, this, &MpvPlayer::doUpdate, Qt::QueuedConnection);
}

MpvPlayer::~MpvPlayer()
{
    if (mpv_gl) // only initialized if something got drawn
    {
        mpv_render_context_free(mpv_gl);
    }

    mpv_terminate_destroy(mpv);
}

qreal MpvPlayer::position()
{
    return m_position;
}

qreal MpvPlayer::duration()
{
    return m_duration;
}

bool MpvPlayer::paused()
{
    return m_paused;
}

void MpvPlayer::play()
{
    if (!paused()) {
        return;
    }
    setProperty("pause", false);
    Q_EMIT pausedChanged();
}

void MpvPlayer::pause()
{
    if (paused()) {
        return;
    }
    setProperty("pause", true);
    Q_EMIT pausedChanged();
}

void MpvPlayer::stop()
{
    setPosition(0);
    setProperty("pause", true);
    Q_EMIT pausedChanged();
}

void MpvPlayer::setPosition(double value)
{
    if (value == position()) {
        return;
    }
    setProperty("time-pos", value);
    Q_EMIT positionChanged();
}

void MpvPlayer::seek(qreal offset)
{
    command(QStringList() << "add"
                          << "time-pos" << QString::number(offset));
}

void MpvPlayer::on_update(void *ctx)
{
    MpvPlayer *self = (MpvPlayer *)ctx;
    Q_EMIT self->onUpdate();
}

// connected to onUpdate(); signal makes sure it runs on the GUI thread
void MpvPlayer::doUpdate()
{
    update();
}

void MpvPlayer::command(const QVariant &params)
{
    mpv::qt::command(mpv, params);
}

void MpvPlayer::setOption(const QString &name, const QVariant &value)
{
    mpv::qt::set_option_variant(mpv, name, value);
}

void MpvPlayer::setProperty(const QString &name, const QVariant &value)
{
    mpv::qt::set_property(mpv, name, value);
}

QVariant MpvPlayer::getProperty(const QString &name)
{
    return mpv::qt::get_property(mpv, name);
}

QQuickFramebufferObject::Renderer *MpvPlayer::createRenderer() const
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    window()->setPersistentOpenGLContext(true);
#endif
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(const_cast<MpvPlayer *>(this));
}

void MpvPlayer::onMpvEvents()
{
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }

        switch (event->event_id) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *prop = (mpv_event_property *)event->data;
            if (strcmp(prop->name, "time-pos") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    double time = *(double *)prop->data;
                    m_position = time;
                    Q_EMIT positionChanged();
                }
            } else if (strcmp(prop->name, "duration") == 0) {
                if (prop->format == MPV_FORMAT_DOUBLE) {
                    double time = *(double *)prop->data;
                    m_duration = time;
                    Q_EMIT durationChanged();
                }
            } else if (strcmp(prop->name, "pause") == 0) {
                if (prop->format == MPV_FORMAT_FLAG) {
                    m_paused = *(bool *)prop->data;
                    Q_EMIT pausedChanged();
                }
            }
            break;
        }
        default:;
            // Ignore uninteresting or unknown events.
        }
    }
}
