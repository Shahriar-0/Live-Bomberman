#include "../include/animation.h"

Animation::Animation(const QString& name,
                     const QList<QPixmap>& frames,
                     int frameDurationMs,
                     QObject* parent)
    : QObject(parent),
    m_frames(frames),
    m_frameDurationMs(frameDurationMs),
    m_currentFrameIndex(0),
    m_timer(std::make_unique<QTimer>(this)),
    m_name(name)
{
    validateFrames();
    setupTimer();
}

void Animation::start()
{
    if (!m_frames.isEmpty() && !m_timer->isActive()) {
        m_timer->start(m_frameDurationMs);
    }
}

void Animation::stop()
{
    m_timer->stop();
}

void Animation::reset()
{
    stop();
    m_currentFrameIndex = 0;
}

bool Animation::isRunning() const
{
    return m_timer->isActive();
}

const QPixmap& Animation::currentFrame() const
{
    static const QPixmap emptyPixmap;
    return m_frames.isEmpty() ? emptyPixmap : m_frames[m_currentFrameIndex];
}

QString Animation::getName() const
{
    return m_name;
}

QList<QPixmap> Animation::loadFrames(const QString& pathTemplate, int frameCount)
{
    QList<QPixmap> frames;
    for (int i = 1; i <= frameCount; ++i) {
        QString framePath = pathTemplate.arg(i);
        QPixmap frame(framePath);
        if (frame.isNull()) {
            qWarning() << "Failed to load frame:" << framePath;
        } else {
            frames.append(frame);
        }
    }
    return frames;
}

void Animation::advanceFrame()
{
    if (!m_frames.isEmpty()) {
        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_frames.size();
        emit frameChanged();
    }
}

void Animation::validateFrames() const
{
    if (m_frames.isEmpty()) {
        qWarning() << "Animation" << m_name << "has no frames.";
    }
}

void Animation::setupTimer()
{
    m_timer->setSingleShot(false);
    connect(m_timer.get(), &QTimer::timeout, this, &Animation::advanceFrame);
}
