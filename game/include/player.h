#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>
#include <QGraphicsColorizeEffect>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <cmath>

#include "animation.h"
#include "block.h"
#include "bomb.h"
#include "gameobject.h"

class Player : public GameObject {
    Q_OBJECT
public:
    explicit Player(const QPixmap& sprite, int playerId, QGraphicsItem* parent = nullptr);
    void takeDamage(int damage);
    int getHealth() const { return m_health; }
    int getPlayerId() const { return m_playerId; }
    void placeBomb();
    void die();
    void updateDirectionState(int key, bool isPressed);
    void setHealth(int health);

public slots:
    void updateMovement();

signals:
    void playerDied(int playerId);
    void playerMoved(int playerId, Qt::Key key, bool isPressed);
    void playerPlacedBomb(int playerId);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void initializeAnimations();
    void applyPlayerEffects();

    bool m_moveUp = false;
    bool m_moveDown = false;
    bool m_moveLeft = false;
    bool m_moveRight = false;
    bool m_isMoving = false;
    bool m_isDead = false;
    static constexpr qreal SCALE_FACTOR = 0.8;
    static constexpr qreal DEFAULT_SPEED = 2.0;
    static constexpr int DEFAULT_HEALTH = 3;
    static constexpr qreal DIAGONAL_FACTOR = 0.7071;
    static constexpr int TILE_SIZE = 16;
    static constexpr int DEATH_ANIMATION_DURATION = 600;
    static constexpr int FRAME_COUNT = 3;
    static constexpr int FRAME_DURATION = 100;
    qreal m_speed = DEFAULT_SPEED;
    int m_health = DEFAULT_HEALTH;
    int m_playerId;
};

#endif // PLAYER_H
