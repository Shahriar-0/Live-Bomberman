#include "../include/player.h"

Player::Player(const QPixmap& sprite, int playerId, QGraphicsItem* parent)
    : GameObject(sprite, parent), m_playerId(playerId) {
    setScale(SCALE_FACTOR);
    applyPlayerEffects();
    initializeAnimations();
}

void Player::applyPlayerEffects() {
    if (m_playerId != 1) {
        QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
        effect->setColor(Qt::red);
        effect->setStrength(1.0);
        setGraphicsEffect(effect);
    }
}

void Player::initializeAnimations() {
    QStringList directions = {"down", "up", "left", "right", "dead"};
    for (const QString& dir : directions) {
        QList<QPixmap> frames = Animation::loadFrames(":/assets/player_" + dir + "%1.png", FRAME_COUNT);
        addAnimation(new Animation(dir, frames, FRAME_DURATION, this));
    }
}

void Player::takeDamage(int damage) {
    if (m_isDead) {
        return;
    }

    m_health -= damage;
    if (m_health <= 0) die();
}

void Player::die() {
    if (m_isDead) {
        return;
    }

    m_isDead = true;
    playAnimation("dead");
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    setEnabled(false);
    QTimer::singleShot(DEATH_ANIMATION_DURATION, this, [this]() {
        if (scene())
            scene()->removeItem(this);
        delete this;
    });
}

void Player::keyPressEvent(QKeyEvent* event) {
    if (m_isDead) {
        event->ignore();
        return;
    }

    if (event->key() == Qt::Key_Space) {
        placeBomb();
        return;
    }

    updateDirectionState(event->key(), true);
    QGraphicsPixmapItem::keyPressEvent(event);
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    if (m_isDead) {
        event->ignore();
        return;
    }
    updateDirectionState(event->key(), false);
    QGraphicsPixmapItem::keyReleaseEvent(event);
}

void Player::updateDirectionState(int key, bool isPressed) {
    switch (key) {
    case Qt::Key_W: m_moveUp = isPressed; break;
    case Qt::Key_S: m_moveDown = isPressed; break;
    case Qt::Key_A: m_moveLeft = isPressed; break;
    case Qt::Key_D: m_moveRight = isPressed; break;
    }
    m_isMoving = m_moveUp || m_moveDown || m_moveLeft || m_moveRight;
}

void Player::updateMovement() {
    if (m_isDead) {
        return;
    }

    qreal dx = (m_moveLeft ? -m_speed : 0) + (m_moveRight ? m_speed : 0);
    qreal dy = (m_moveUp ? -m_speed : 0) + (m_moveDown ? m_speed : 0);
    if (dx != 0 && dy != 0) {
        dx *= DIAGONAL_FACTOR;
        dy *= DIAGONAL_FACTOR;
    }
    if (dx == 0 && dy == 0) {
        stopAnimation();
        return;
    }

    QPointF newPos = pos() + QPointF(dx, dy);
    setPos(newPos);
    for (QGraphicsItem* item : collidingItems()) {
        if (dynamic_cast<Block*>(item)) {
            setPos(pos() - QPointF(dx, dy));
            stopAnimation();
            return;
        }
    }
    playAnimation((std::abs(dy) >= std::abs(dx)) ? (dy < 0 ? "up" : "down") : (dx < 0 ? "left" : "right"));
}

void Player::placeBomb() {
    if (!scene()) {
        return;
    }
    
    QPointF center = pos() + boundingRect().center();
    qreal tileX = std::floor(center.x() / TILE_SIZE) * TILE_SIZE;
    qreal tileY = std::floor(center.y() / TILE_SIZE) * TILE_SIZE;
    Bomb* bomb = new Bomb();
    bomb->setPos(tileX, tileY);
    bomb->setZValue(zValue() - 1);
    for (QGraphicsItem* item : scene()->items(bomb->boundingRect().translated(bomb->pos()))) {
        if (dynamic_cast<Block*>(item)) {
            delete bomb;
            return;
        }
    }
    scene()->addItem(bomb);
}
