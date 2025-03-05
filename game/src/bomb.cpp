#include "../include/bomb.h"

Bomb::Bomb(QGraphicsItem* parent)
    : GameObject(QPixmap(":/assets/bomb1.png"), parent),
      m_explosionPath(":/assets/bomb%1.png") {
    QList<QPixmap> explosionFrames = Animation::loadFrames(m_explosionPath, FRAME_COUNT);
    Animation* explosionAnim = new Animation("explode", explosionFrames, 250, this);
    addAnimation(explosionAnim);
    playAnimation("explode");
    m_explosionTimer = new QTimer(this);
    m_explosionTimer->setSingleShot(true);
    connect(m_explosionTimer, &QTimer::timeout, this, &Bomb::explode);
    m_explosionTimer->start(EXPLOSION_DELAY);
}

void Bomb::explode() {
    QList<QRectF> explosionAreas;
    spawnCenterExplosion(explosionAreas);
    propagateExplosionDirection(QPointF(-1, 0), Left, EndLeft, explosionAreas);
    propagateExplosionDirection(QPointF(1, 0), Right, EndRight, explosionAreas);
    propagateExplosionDirection(QPointF(0, -1), Up, EndUp, explosionAreas);
    propagateExplosionDirection(QPointF(0, 1), Down, EndDown, explosionAreas);
    applyDamageToPlayers(explosionAreas);
    QTimer::singleShot(BOMB_REMOVAL_DELAY, this, &Bomb::deleteLater);
}

void Bomb::spawnCenterExplosion(QList<QRectF>& explosionAreas) const {
    ExplosionEffect* centerExp = new ExplosionEffect(pos(), Center, parentItem());
    scene()->addItem(centerExp);
    explosionAreas.append(QRectF(pos(), QSizeF(TILE_SIZE, TILE_SIZE)));
}

void Bomb::propagateExplosionDirection(const QPointF& offset, ExplosionType normalType,
                                       ExplosionType endType, QList<QRectF>& explosionAreas) const {
    QList<QPointF> validTiles;
    for (int i = 1; i <= EXPLOSION_RANGE; ++i) {
        const QPointF tilePos = pos() + (offset * i * TILE_SIZE);
        bool blocked = false;
        QList<QGraphicsItem*> itemsInTile = scene()->items(QRectF(tilePos, QSizeF(TILE_SIZE, TILE_SIZE)));
        for (QGraphicsItem* item : itemsInTile) {
            if (Block* block = dynamic_cast<Block*>(item)) {
                if (BreakableBlock* bBlock = dynamic_cast<BreakableBlock*>(block))
                    bBlock->destroyBlock();
                blocked = true;
                break;
            }
        }
        if (blocked) {
            break;
        }
        validTiles.append(tilePos);
    }
    for (int i = 0; i < validTiles.size(); ++i) {
        const ExplosionType type = (i == validTiles.size() - 1) ? endType : normalType;
        ExplosionEffect* expEff = new ExplosionEffect(validTiles[i], type, parentItem());
        scene()->addItem(expEff);
        explosionAreas.append(QRectF(validTiles[i], QSizeF(TILE_SIZE, TILE_SIZE)));
    }
}

void Bomb::applyDamageToPlayers(const QList<QRectF>& explosionAreas) const {
    for (QGraphicsItem* item : scene()->items()) {
        if (Player* player = dynamic_cast<Player*>(item)) {
            if (!player->isEnabled())
                continue;
            const QRectF playerRect = player->sceneBoundingRect();
            for (const QRectF& area : explosionAreas) {
                if (area.intersects(playerRect)) {
                    player->takeDamage(1);
                    break;
                }
            }
        }
    }
}

QPainterPath Bomb::shape() const {
    return QPainterPath();
}
