// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <MpvAbstractItem>

class MpvPlayer : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal position READ position NOTIFY positionChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool looping READ looping WRITE setLooping NOTIFY loopingChanged)
    Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged)
    Q_PROPERTY(bool stopped READ stopped NOTIFY stoppedChanged)

public:
    explicit MpvPlayer(QQuickItem *parent = nullptr);
    ~MpvPlayer() = default;

    qreal position() const;
    qreal duration() const;
    bool paused() const;
    QSize sourceSize() const;
    QString source() const;
    bool loading() const;
    bool looping() const;
    bool autoPlay() const;
    bool stopped() const;

    void setSource(const QString &source);
    void setLooping(bool loop);
    void setAutoPlay(bool autoPlay);

public Q_SLOTS:
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setPosition(double value);
    void seek(qreal offset);

Q_SIGNALS:
    void positionChanged();
    void durationChanged();
    void pausedChanged();
    void onUpdate();
    void sourceSizeChanged();
    void sourceChanged();
    void loadingChanged();
    void loopingChanged();
    void autoPlayChanged();
    void stoppedChanged();

private:
    void onPropertyChanged(const QString &property, const QVariant &value);

    bool m_paused = true;
    qreal m_position = 0;
    qreal m_duration = 0;
    QSize m_sourceSize;
    QString m_source;
    QString m_currentSource;
    bool m_loading = false;
    bool m_looping = false;
    bool m_autoPlay = false;
};