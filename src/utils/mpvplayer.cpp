// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mpvplayer.h"

#include <MpvController>

MpvPlayer::MpvPlayer(QQuickItem *parent)
    : MpvAbstractItem(parent)
{
    // enable console output
    setProperty(QStringLiteral("terminal"), QStringLiteral("yes"));

    // don't load user scripts or configs
    setProperty(QStringLiteral("config"), QStringLiteral("no"));
    setProperty(QStringLiteral("load-scripts"), QStringLiteral("no"));

    // force vo to libmpv (which it should be set to anyway)
    setProperty(QStringLiteral("vo"), QStringLiteral("libmpv"));

    // use safe hardware acceleration
    setProperty(QStringLiteral("hwdec"), QStringLiteral("auto-safe"));

    // disable OSD and fonts
    setProperty(QStringLiteral("osd-level"), QStringLiteral("0"));
    setProperty(QStringLiteral("embeddedfonts"), QStringLiteral("no"));

    // disable input
    setProperty(QStringLiteral("input-builtin-bindings"), QStringLiteral("no"));
    setProperty(QStringLiteral("input-default-bindings"), QStringLiteral("no"));
    setProperty(QStringLiteral("input-vo-keyboard"), QStringLiteral("no"));

    observeProperty(QStringLiteral("duration"), MPV_FORMAT_DOUBLE);
    observeProperty(QStringLiteral("time-pos"), MPV_FORMAT_DOUBLE);
    observeProperty(QStringLiteral("pause"), MPV_FORMAT_FLAG);

    connect(mpvController(), &MpvController::propertyChanged, this, &MpvPlayer::onPropertyChanged, Qt::QueuedConnection);
    connect(
        mpvController(),
        &MpvController::fileLoaded,
        this,
        [this] {
            m_loading = false;
            Q_EMIT loadingChanged();
        },
        Qt::QueuedConnection);
    connect(
        mpvController(),
        &MpvController::videoReconfig,
        this,
        [this] {
            const int64_t w = getProperty(QStringLiteral("dwidth")).toInt();
            const int64_t h = getProperty(QStringLiteral("dheight")).toInt();
            if (w > 0 && h > 0) {
                const QSize newSize(w, h);
                if (newSize != m_sourceSize) {
                    m_sourceSize = newSize;
                    Q_EMIT sourceSizeChanged();
                }
            }
        },
        Qt::QueuedConnection);
}

qreal MpvPlayer::position() const
{
    return m_position;
}

qreal MpvPlayer::duration() const
{
    return m_duration;
}

bool MpvPlayer::paused() const
{
    return m_paused;
}

QSize MpvPlayer::sourceSize() const
{
    return m_sourceSize;
}

QString MpvPlayer::source() const
{
    return m_source;
}

bool MpvPlayer::loading() const
{
    return m_loading;
}

bool MpvPlayer::looping() const
{
    return m_looping;
}

bool MpvPlayer::autoPlay() const
{
    return m_autoPlay;
}

bool MpvPlayer::stopped() const
{
    return m_currentSource.isEmpty();
}

void MpvPlayer::setSource(const QString &source)
{
    if (m_source == source) {
        return;
    }

    m_source = source;

    if (m_autoPlay) {
        play();
    }

    Q_EMIT sourceChanged();
}

void MpvPlayer::setLooping(bool loop)
{
    if (m_looping == loop) {
        return;
    }

    m_looping = loop;
    setProperty(QStringLiteral("loop-file"), loop ? QStringLiteral("inf") : QStringLiteral("no"));

    Q_EMIT loopingChanged();
}

void MpvPlayer::setAutoPlay(bool autoPlay)
{
    if (m_autoPlay == autoPlay) {
        return;
    }

    m_autoPlay = autoPlay;

    Q_EMIT autoPlayChanged();
}

void MpvPlayer::play()
{
    if (!paused() || m_source.isEmpty()) {
        return;
    }

    // setSource doesn't actually load the file into MPV, because the player is always created
    // regardless of autoPlay. This incurs a very visible overhead in the UI, so we want to delay
    // the loadfile command until the last possible moment (when the user requests to play it).
    if (m_currentSource != m_source) {
        m_loading = true;
        Q_EMIT loadingChanged();

        command(QStringList{QStringLiteral("loadfile"), m_source});

        m_currentSource = m_source;
        m_paused = false;

        Q_EMIT stoppedChanged();
        Q_EMIT pausedChanged();
    }

    setProperty(QStringLiteral("pause"), false);
}

void MpvPlayer::pause()
{
    if (paused()) {
        return;
    }
    setProperty(QStringLiteral("pause"), true);
}

void MpvPlayer::stop()
{
    setPosition(0);
    setProperty(QStringLiteral("pause"), true);
}

void MpvPlayer::setPosition(double value)
{
    if (value == position()) {
        return;
    }
    setProperty(QStringLiteral("time-pos"), value);
}

void MpvPlayer::seek(qreal offset)
{
    command(QStringList() << QStringLiteral("add") << QStringLiteral("time-pos") << QString::number(offset));
}

void MpvPlayer::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == QStringLiteral("time-pos")) {
        m_position = value.toDouble();
        Q_EMIT positionChanged();
    } else if (property == QStringLiteral("duration")) {
        m_duration = value.toDouble();
        Q_EMIT durationChanged();
    } else if (property == QStringLiteral("pause")) {
        // pause is expected to be false if there's no currently loaded media, so skip it
        if (!m_currentSource.isEmpty()) {
            m_paused = value.toBool();
            Q_EMIT pausedChanged();
        }
    }
}

#include "moc_mpvplayer.cpp"