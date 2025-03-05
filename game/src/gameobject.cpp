#include "../include/gameobject.h"

GameObject::GameObject(const QPixmap& sprite, QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(sprite, parent), m_currentAnimation(nullptr) {
    setFlag(QGraphicsItem::ItemIsFocusable, false);
}

void GameObject::addAnimation(Animation* animation) {
    if (!animation) {
        return;
    }
    m_animations.insert(animation->getName(), animation);
    connect(animation, &Animation::frameChanged, this, &GameObject::updateFrame);
}

void GameObject::playAnimation(const QString& name) {
    auto it = m_animations.find(name);
    if (it == m_animations.end()) {
        qWarning() << "Animation" << name << "not found!";
        return;
    }

    if (m_currentAnimation == it.value() && m_currentAnimation->isRunning()) {
        return;
    }

    if (m_currentAnimation) {
        m_currentAnimation->stop();
    }

    m_currentAnimation = it.value();
    m_currentAnimation->reset();
    m_currentAnimation->start();
    setPixmap(m_currentAnimation->currentFrame());
}

void GameObject::stopAnimation() {
    if (m_currentAnimation) {
        m_currentAnimation->stop();
        m_currentAnimation = nullptr;
    }
}

void GameObject::updateFrame() {
    if (m_currentAnimation) {
        setPixmap(m_currentAnimation->currentFrame());
    }
}
