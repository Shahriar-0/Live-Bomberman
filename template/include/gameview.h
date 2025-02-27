#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>
#include <QBrush>


class GameView {
public:
    explicit GameView(QObject *parent = nullptr);
    QGraphicsScene* scene() const { return m_scene; }
    QGraphicsView* view() const { return m_view; }
    void initialize();

private:
    QGraphicsScene *m_scene;
    QGraphicsView  *m_view;
};

#endif // GAMEVIEW_H
