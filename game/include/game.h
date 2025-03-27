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
#include "networkmanager.h"
#include "player.h"
#include "gameNetworkManager.h"
#include "maploader.h"

class QTimer;
class MapLoader;
class GameView;
class HUD;

class Game : public QObject {
    Q_OBJECT

public:
    explicit Game(int selectedPlayer, const QString& protocol, QObject* parent = nullptr);
    void start();
    void addPlayer(Player* player);

public slots:
    void update();

private slots:
void handlePlayerDied(int playerId);
void handlePlayerMoved(int playerId, int key, bool isPressed);
void handlePlayerPlacedBomb(int playerId);

private:
    void connectGameTimer();
    void loadMap();
    void setFocusOnPlayer();
    void connectNetworkSignals();
    void connectPlayerSignals(QPointer<Player> player);
    void gameOver(int diedPlayerId);
    QPointer<Player> getPlayerById(int playerId);

    GameView* m_gameView;
    QTimer* gameTimer;
    MapLoader* mapLoader;
    HUD* hud;
    GameNetworkManager* m_gameNetworkManager;

    QString protocol;
    int selectedPlayer;

    QList<QPointer<Player>> players;
    static constexpr int FrameRate = 30;
};

#endif // GAME_H
