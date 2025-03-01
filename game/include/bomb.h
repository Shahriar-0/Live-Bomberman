#ifndef BOMB_H
#define BOMB_H

#include "gameobject.h"
#include "explosioneffect.h"
#include "animation.h"
#include "block.h"
#include "player.h"
#include "breakableblock.h"
#include <QTimer>
#include <QPainterPath>
#include <QList>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QSizeF>
#include <QPixmap>
#include <QPointF>
#include <QDebug>
#include <cmath>

class Bomb : public GameObject {
    Q_OBJECT
public:
    explicit Bomb(QGraphicsItem *parent = nullptr);
public slots:
    void explode();
protected:
    QPainterPath shape() const override;
private:
    void spawnCenterExplosion(QList<QRectF>& explosionAreas) const;
    void propagateExplosionDirection(const QPointF &offset, ExplosionType normalType, ExplosionType endType, QList<QRectF>& explosionAreas) const;
    void applyDamageToPlayers(const QList<QRectF>& explosionAreas) const;
    QString m_explosionPath;
    QTimer *m_explosionTimer;
    static constexpr int FRAME_COUNT = 3;
    static constexpr int EXPLOSION_DELAY = 2000;
    static constexpr int TILE_SIZE = 16;
    static constexpr int EXPLOSION_RANGE = 3;
    static constexpr int BOMB_REMOVAL_DELAY = 200;
};

#endif // BOMB_H
