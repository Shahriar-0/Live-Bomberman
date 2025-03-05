#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QMap>
#include <QObject>
#include <QPixmap>

#include "animation.h"

class GameObject : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    explicit GameObject(const QPixmap& sprite, QGraphicsItem* parent = nullptr);

    void addAnimation(Animation* animation);
    void playAnimation(const QString& name);
    void stopAnimation();

private slots:
    void updateFrame();

private:
    QMap<QString, Animation*> m_animations;
    Animation* m_currentAnimation = nullptr;
};

#endif // GAMEOBJECT_H
