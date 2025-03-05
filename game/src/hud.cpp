#include "../include/hud.h"

HUD::HUD(QGraphicsScene* scene, int sceneWidth) {
    m_healthDisplayP1 = createHealthDisplay("Player 1: ", Qt::blue, QPointF(0, HealthDisplayOffset));
    m_healthDisplayP2 = createHealthDisplay("Player 2: ", Qt::red, QPointF(sceneWidth - HealthDisplayP2Offset, HealthDisplayOffset));
    scene->addItem(m_healthDisplayP1);
    scene->addItem(m_healthDisplayP2);
}

QGraphicsTextItem* HUD::createHealthDisplay(const QString& defaultText, const QColor& textColor, const QPointF& position) {
    QFont font("Arial", 8);
    font.setBold(true);

    auto* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setColor(Qt::black);
    shadowEffect->setBlurRadius(1);
    shadowEffect->setOffset(1, 1);

    QGraphicsTextItem* item = new QGraphicsTextItem(defaultText);
    item->setDefaultTextColor(textColor);
    item->setFont(font);
    item->setPos(position);
    item->setGraphicsEffect(shadowEffect);
    item->setZValue(1);
    return item;
}

void HUD::updateHealth(const QList<QPointer<Player>>& players) {
    QString textP1, textP2;
    for (const QPointer<Player>& player : players) {
        if (player) {
            if (player->getPlayerId() == 1)
                textP1 = QString("Player 1: %1").arg(player->getHealth());
            else if (player->getPlayerId() == 2)
                textP2 = QString("Player 2: %1").arg(player->getHealth());
        }
    }
    m_healthDisplayP1->setPlainText(textP1);
    m_healthDisplayP2->setPlainText(textP2);
}
