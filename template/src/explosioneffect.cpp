#include "../include/explosioneffect.h"

ExplosionEffect::ExplosionEffect(const QPointF &position, ExplosionType type, QGraphicsItem *parent)
    : GameObject(QPixmap(), parent),
    m_type(type)
{
    setPos(position);
    setupExplosionAnimation();
    QTimer::singleShot(EffectRemovalDelayMs, this, &ExplosionEffect::removeEffect);
}

void ExplosionEffect::setupExplosionAnimation() {
    QString framePath = getFramePathForType(m_type);
    Animation *expAnim = new Animation("exp", Animation::loadFrames(framePath, FrameCount), FrameDurationMs, this);
    addAnimation(expAnim);
    playAnimation("exp");
}

QString ExplosionEffect::getFramePathForType(ExplosionType type) const {
    switch (type) {
    case Center:
        return ":/assets/center_exp%1.png";
    case Left:
    case Right:
        return ":/assets/x_exp%1.png";
    case Up:
    case Down:
        return ":/assets/y_exp%1.png";
    case EndLeft:
        return ":/assets/left_exp%1.png";
    case EndRight:
        return ":/assets/right_exp%1.png";
    case EndUp:
        return ":/assets/up_exp%1.png";
    case EndDown:
        return ":/assets/down_exp%1.png";
    default:
        return ":/assets/center_exp%1.png";
    }
}

void ExplosionEffect::removeEffect() {
    deleteLater();
}
