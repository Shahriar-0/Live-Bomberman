#include "../include/gameview.h"

GameView::GameView(QObject *parent)
    : m_scene(new QGraphicsScene(parent)),
    m_view(new QGraphicsView(m_scene))
{
}

void GameView::initialize() {
    QPixmap bgImage(":/assets/Background.png");
    QSize bgSize = bgImage.size();
    m_scene->setBackgroundBrush(QBrush(bgImage));
    m_scene->setSceneRect(0, 0, bgSize.width(), bgSize.height());

    constexpr qreal ScaleFactor = 2.0;
    QSize scaledSize = bgSize * ScaleFactor;
    m_view->setFixedSize(scaledSize);
    m_view->scale(ScaleFactor, ScaleFactor);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    m_view->show();
}
