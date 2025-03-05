#ifndef HUD_H
#define HUD_H

#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QList>
#include <QPointF>
#include <QPointer>
#include <QString>

#include "player.h"

class QGraphicsScene;

class HUD {
public:
    HUD(QGraphicsScene* scene, int sceneWidth);
    void updateHealth(const QList<QPointer<Player>>& players);

private:
    QGraphicsTextItem* createHealthDisplay(const QString& defaultText,
                                           const QColor& textColor,
                                           const QPointF& position);
    QGraphicsTextItem* m_healthDisplayP1;
    QGraphicsTextItem* m_healthDisplayP2;
    static constexpr int HealthDisplayOffset = -3;
    static constexpr int HealthDisplayP2Offset = 64;
};

#endif // HUD_H
