#ifndef EXPLOSIONEFFECT_H
#define EXPLOSIONEFFECT_H

#include "gameobject.h"
#include "animation.h"
#include <QPixmap>
#include <QTimer>

enum ExplosionType {
    Center,
    Left,
    Right,
    Up,
    Down,
    EndLeft,
    EndRight,
    EndUp,
    EndDown
};

class ExplosionEffect : public GameObject {
    Q_OBJECT

public:
    explicit ExplosionEffect(const QPointF &position, ExplosionType type, QGraphicsItem *parent = nullptr);

private slots:
    void removeEffect();

private:
    void setupExplosionAnimation();
    QString getFramePathForType(ExplosionType type) const;

    ExplosionType m_type;
    static constexpr int FrameCount = 2;
    static constexpr int FrameDurationMs = 100;
    static constexpr int EffectRemovalDelayMs = 300;
};

#endif // EXPLOSIONEFFECT_H
