#ifndef ANIMATION_H
#define ANIMATION_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <memory>

class Animation : public QObject {
    Q_OBJECT

public:
    explicit Animation(const QString& name,
                       const QList<QPixmap>& frames,
                       int frameDurationMs,
                       QObject* parent = nullptr);

    void start();
    void stop();
    void reset();
    bool isRunning() const;
    const QPixmap& currentFrame() const;
    QString getName() const;

    static QList<QPixmap> loadFrames(const QString& pathTemplate, int frameCount);

signals:
    void frameChanged();

private slots:
    void advanceFrame();

private:
    void validateFrames() const;
    void setupTimer();

    QList<QPixmap> m_frames;
    int m_frameDurationMs;
    int m_currentFrameIndex = 0;
    std::unique_ptr<QTimer> m_timer;
    QString m_name;
};

#endif // ANIMATION_H
