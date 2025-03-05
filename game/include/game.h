#ifndef GAME_H
#define GAME_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QPointer>
#include <QTimer>

#include "gameview.h"
#include "hud.h"
#include "maploader.h"
#include "player.h"

class QTimer;
class MapLoader;
class GameView;
class HUD;

class Game : public QObject {
    Q_OBJECT
public:
    explicit Game(int selectedPlayer, const QString& protocol, QObject* parent = nullptr);
    void start();
    QList<QPointer<Player>> players;

public slots:
    void update();

private:
    void connectGameTimer();
    void loadMap();
    void setFocusOnPlayer();

    GameView* m_gameView;
    QTimer* gameTimer;
    MapLoader* mapLoader;
    HUD* hud;

    int selectedPlayer;
    QString protocol;
    static constexpr int FrameRate = 30;
};

#endif // GAME_H
